#include "GHRenderDeviceDX12.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX12Helpers.h"
#include "GHWin32/GHWin32Window.h"
#include "GHDX12Fence.h"
#include "Render/GHCamera.h"
#include "Render/GHRenderTarget.h"

#define DEBUG_DX12 1

GHRenderDeviceDX12::GHRenderDeviceDX12(GHWin32Window& window)
	: mWindow(window)
	, mClearColor(0, 0, 0, 0)
{
#ifdef DEBUG_DX12
	Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
	debugInterface->EnableDebugLayer();
#endif

	Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter = GHDX12Helpers::findDX12Adapter();
	HRESULT deviceRes = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mDXDevice));
	if (deviceRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create d3d12 device");
	}

#ifdef DEBUG_DX12
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
	if (mDXDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)) == S_OK)
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
		D3D12_INFO_QUEUE_FILTER filter = {};
		D3D12_MESSAGE_ID suppressedMessages[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
		};
		filter.DenyList.NumIDs = _countof(suppressedMessages);
		filter.DenyList.pIDList = suppressedMessages;
		D3D12_MESSAGE_SEVERITY infoSev = D3D12_MESSAGE_SEVERITY_INFO;
		filter.DenyList.NumSeverities = 1;
		filter.DenyList.pSeverityList = &infoSev;
		infoQueue->PushStorageFilter(&filter);
	}
#endif

	mDXCommandQueue = GHDX12Helpers::createCommandQueue(mDXDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
	const GHPoint2i& screenSize = mWindow.getClientAreaSize();
	mDXSwapChainSampleDesc = { 1, 0 };
	mDXSwapChain = GHDX12Helpers::createSwapChain(mWindow.getHWND(), mDXCommandQueue, screenSize[0], screenSize[1], NUM_SWAP_BUFFERS, SWAP_BUFFER_FORMAT, mDXSwapChainSampleDesc);
	const int numDescriptors = 256;
	mDXDescriptorHeap = GHDX12Helpers::createDescriptorHeap(mDXDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, numDescriptors);

	for (uint32_t frameId = 0; frameId < NUM_SWAP_BUFFERS; ++frameId)
	{
		GHDX12Helpers::createBackBuffer(mDXDevice, mDXSwapChain, mDXDescriptorHeap, mFrameBackends[frameId].mBackBuffer, frameId);
		mFrameBackends[frameId].mCommandList = new GHDX12CommandList(mDXDevice, mDXCommandQueue);

		// msaa target
		{
			D3D12_RESOURCE_DESC texDesc;
			GHDX12Helpers::createTexture2dDesc(texDesc, screenSize[0], screenSize[1]);
			texDesc.Format = SWAP_BUFFER_FORMAT;
			texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			texDesc.SampleDesc.Count = MSAA_SAMPLE_COUNT;
			texDesc.SampleDesc.Quality = MSAA_SAMPLE_QUALITY;
			texDesc.MipLevels = 1;

			D3D12_HEAP_PROPERTIES heapProperties;
			GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_DEFAULT);

			HRESULT hr = getDXDevice()->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&texDesc,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				nullptr,
				IID_PPV_ARGS(&mFrameBackends[frameId].mMsaaBuffer)
			);
			if (FAILED(hr))
			{
				GHDebugMessage::outputString("Failed to create swap msaa buffer");
				return;
			}
			mFrameBackends[frameId].mMsaaBuffer->SetName(L"Swap MSAA Buffer");
		}

		auto rtvDescriptorSize = mDXDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mDXDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += rtvDescriptorSize * frameId;
		mFrameBackends[frameId].mBackBufferRTV = rtvHandle;

		getDXDevice()->CreateRenderTargetView(mFrameBackends[frameId].mMsaaBuffer.Get(), nullptr, rtvHandle);
	}
	mUploadCommandList = new GHDX12CommandList(mDXDevice, mDXCommandQueue);
	mComputeCommandList = new GHDX12CommandList(mDXDevice, mDXCommandQueue);
	createDepthBuffer();

	mScissorRect.left = 0;
	mScissorRect.right = screenSize[0];
	mScissorRect.top = 0;
	mScissorRect.bottom = screenSize[1];

	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = (FLOAT)screenSize[0];
	mViewport.Height = (FLOAT)screenSize[1];
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	createGraphicsRootSignature();
}

GHRenderDeviceDX12::~GHRenderDeviceDX12(void)
{
	flushGPU();
	if (mDXSwapChain) mDXSwapChain->SetFullscreenState(false, NULL);
	for (size_t frameId = 0; frameId < NUM_SWAP_BUFFERS; ++frameId)
	{
		if (mFrameBackends[frameId].mCommandList) delete mFrameBackends[frameId].mCommandList;
	}
	delete mComputeCommandList;
}

void GHRenderDeviceDX12::reinit(void)
{
	// todo.
}

static uint32_t getNextBackendId(uint32_t currId)
{
	uint32_t ret = currId+1;
	if (ret >= NUM_SWAP_BUFFERS)
	{
		ret = 0;
	}
	return ret;
}

bool GHRenderDeviceDX12::beginFrame(void)
{
	// wait for the next frame to become available.
	mCurrBackend = getNextBackendId(mCurrBackend);
	mFrameBackends[mCurrBackend].mCommandList->waitForCompletion();
	mFrameBackends[mCurrBackend].mCommandList->begin();
	mUploadCommandList->getDXCommandList()->SetName(L"Render CL");

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mFrameBackends[mCurrBackend].mBackBuffer.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->ResourceBarrier(1, &barrier);

	D3D12_RESOURCE_BARRIER msaaBarrier;
	msaaBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	msaaBarrier.Transition.pResource = mFrameBackends[mCurrBackend].mMsaaBuffer.Get();
	msaaBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
	msaaBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	msaaBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	msaaBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->ResourceBarrier(1, &msaaBarrier);

	getRenderCommandList()->SetGraphicsRootSignature(mGraphicsRootSignature.Get());
	getRenderCommandList()->RSSetScissorRects(1, &mScissorRect); 

	applyDefaultTarget();
	clearBuffers();

	return true;
}

void GHRenderDeviceDX12::endFrame(void)
{
	// resolve msaa
	{
		// transition resources to the resolve ready state.
		D3D12_RESOURCE_BARRIER srcInBarrier;
		srcInBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		srcInBarrier.Transition.pResource = mFrameBackends[mCurrBackend].mMsaaBuffer.Get();
		srcInBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		srcInBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		srcInBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		srcInBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->ResourceBarrier(1, &srcInBarrier);
		D3D12_RESOURCE_BARRIER dstInBarrier;
		dstInBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		dstInBarrier.Transition.pResource = mFrameBackends[mCurrBackend].mBackBuffer.Get();
		dstInBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		dstInBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST;
		dstInBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		dstInBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->ResourceBarrier(1, &dstInBarrier);

		// resolve.
		mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->ResolveSubresource(mFrameBackends[mCurrBackend].mBackBuffer.Get(), 0, mFrameBackends[mCurrBackend].mMsaaBuffer.Get(), 0, SWAP_BUFFER_FORMAT);

	}

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mFrameBackends[mCurrBackend].mBackBuffer.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->ResourceBarrier(1, &barrier);

	mFrameBackends[mCurrBackend].mCommandList->endAndSubmit();

	// possible todo: apply DXGI_PRESENT_ALLOW_TEARING
	int vsync = mVSyncEnabled ? 1 : 0;
	mDXSwapChain->Present(vsync, 0);
}

void GHRenderDeviceDX12::beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers)
{
	mRenderTarget = optionalTarget;
	setClearColor(clearColor);
	if (mRenderTarget)
	{
		mRenderTarget->apply();
	}
	if (clearBuffers)
	{
		this->clearBuffers();
	}
}

void GHRenderDeviceDX12::endRenderPass(void)
{
	if (mRenderTarget)
	{
		mRenderTarget->remove();
		mRenderTarget = 0;
	}
}

void GHRenderDeviceDX12::applyDefaultTarget(void)
{
	GHDX12RTGroup rtGroup;
	rtGroup.mRt0 = mFrameBackends[mCurrBackend].mBackBufferRTV;
	rtGroup.mRt0Format = SWAP_BUFFER_FORMAT;
	rtGroup.mDepth = mDepthDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtGroup.mDepthFormat = DEPTH_BUFFER_FORMAT;
	rtGroup.mSampleCount = MSAA_SAMPLE_COUNT;
	rtGroup.mSampleQuality = MSAA_SAMPLE_QUALITY;

	applyRenderTarget(rtGroup);
	getRenderCommandList()->RSSetViewports(1, &mViewport);
}

void GHRenderDeviceDX12::applyRenderTarget(const GHDX12RTGroup& rtGroup)
{
	mActiveRTGroup = rtGroup;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDepthDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->OMSetRenderTargets(1, &mActiveRTGroup.mRt0, FALSE, &mActiveRTGroup.mDepth);
}

void GHRenderDeviceDX12::applyViewInfo(const GHViewInfo& viewInfo)
{
	mViewInfo = viewInfo;
	// todo: viewport.
}

void GHRenderDeviceDX12::createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
	GHViewInfo::ViewTransforms& deviceTransforms,
	const GHCamera& camera, bool isRenderToTexture) const
{
	deviceTransforms = engineTransforms;

	deviceTransforms.mPlatformGUITrans = GHTransform(
		2.0, 0.0, 0.0, 0.0,
		0.0, -2.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-1.0, 1.0, 0.0, 1.0
	);

	if (camera.getIsOrtho())
	{
		//D3DXMatrixOrthoRH 
		deviceTransforms.mProjectionTransform.becomeIdentity();
		deviceTransforms.mProjectionTransform.getMatrix()[0] = 2.0f / camera.getOrthoWidth();
		deviceTransforms.mProjectionTransform.getMatrix()[5] = 2.0f / camera.getOrthoHeight();
		deviceTransforms.mProjectionTransform.getMatrix()[10] = 1.0f / (camera.getNearClip() - camera.getFarClip());
		deviceTransforms.mProjectionTransform.getMatrix()[14] = camera.getNearClip() / (camera.getNearClip() - camera.getFarClip());

		deviceTransforms.mViewTransform.mult(deviceTransforms.mProjectionTransform,
			deviceTransforms.mViewProjTransform);
	}

	// d3d is our primary projection platform, so render to texture is the same as render to screen.
}

const GHViewInfo& GHRenderDeviceDX12::getViewInfo(void) const
{
	return mViewInfo;
}

void GHRenderDeviceDX12::handleGraphicsQualityChange(void)
{
	// todo
}

GHTexture* GHRenderDeviceDX12::resolveBackbuffer(void)
{
	// todo
	return 0;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GHRenderDeviceDX12::beginUploadCommandList(void)
{
	mUploadCommandList->begin();
	mUploadCommandList->getDXCommandList()->SetName(L"Upload CL");
	return mUploadCommandList->getDXCommandList();
}

void GHRenderDeviceDX12::endUploadCommandList(void)
{
	mUploadCommandList->endAndSubmit();
	// wait for it to finish here to make sure everything is available.
	// if this is a bottleneck we can figure out something else.
	mUploadCommandList->waitForCompletion();
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GHRenderDeviceDX12::beginComputeCommandList(void)
{
	mComputeCommandList->begin();
	mUploadCommandList->getDXCommandList()->SetName(L"Compute CL");
	return mComputeCommandList->getDXCommandList();
}

void GHRenderDeviceDX12::endComputeCommandList(void)
{
	mComputeCommandList->endAndSubmit();
	// chances are we really don't want to wait for completion here.
	// will have to refactor this stuff when we want to generate mipmaps for render targets.
	// need to keep the descriptor heap around for the right period of time in order to not wait here.
	mComputeCommandList->waitForCompletion();
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GHRenderDeviceDX12::getRenderCommandList(void)
{
	return mFrameBackends[mCurrBackend].mCommandList->getDXCommandList();
}

void GHRenderDeviceDX12::createGraphicsRootSignature(void)
{
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumStaticSamplers = 0;
	desc.pStaticSamplers = nullptr;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// Create two descriptor tables for the cbuffers.  one visible by vertex and one visible by pixel.
	// also one for the pixel srv (add one for vertex?)
	D3D12_ROOT_PARAMETER params[4];
	desc.NumParameters = 4;
	desc.pParameters = &params[0];

	// vertex cbuffers
	params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	// pixel cbuffers
	params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// pixel textures
	params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// pixel samplers
	params[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_DESCRIPTOR_RANGE descRange;
	descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descRange.NumDescriptors = 4;
	descRange.BaseShaderRegister = 0;
	descRange.RegisterSpace = 0;
	descRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	params[0].DescriptorTable.NumDescriptorRanges = 1;
	params[0].DescriptorTable.pDescriptorRanges = &descRange;
	params[1].DescriptorTable.NumDescriptorRanges = 1;
	params[1].DescriptorTable.pDescriptorRanges = &descRange;

	D3D12_DESCRIPTOR_RANGE srvDescRange;
	srvDescRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescRange.NumDescriptors = MAX_TEXTURES;
	srvDescRange.BaseShaderRegister = 0;
	srvDescRange.RegisterSpace = 0;
	srvDescRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	params[2].DescriptorTable.NumDescriptorRanges = 1;
	params[2].DescriptorTable.pDescriptorRanges = &srvDescRange;

	D3D12_DESCRIPTOR_RANGE samplerDescRange;
	samplerDescRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	samplerDescRange.NumDescriptors = MAX_TEXTURES;
	samplerDescRange.BaseShaderRegister = 0;
	samplerDescRange.RegisterSpace = 0;
	samplerDescRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	params[3].DescriptorTable.NumDescriptorRanges = 1;
	params[3].DescriptorTable.pDescriptorRanges = &samplerDescRange;

	desc.NumStaticSamplers = 0;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = 0;
	Microsoft::WRL::ComPtr<ID3DBlob> blobError = 0;
	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), blobError.GetAddressOf());
	if (FAILED(hr))
	{
		const char* errorBuffer = "";
		if (blobError)
		{
			errorBuffer = (const char*)blobError->GetBufferPointer();
		}
		GHDebugMessage::outputString("Failed to create root signature blob: %s", errorBuffer);
		return;
	}

	auto blobBuffer = signatureBlob->GetBufferPointer();
	auto blobBufferSize = signatureBlob->GetBufferSize();
	hr = getDXDevice()->CreateRootSignature(0, blobBuffer, blobBufferSize, IID_PPV_ARGS(&mGraphicsRootSignature));
	if (FAILED(hr))
	{
		GHDebugMessage::outputString("Failed to create root signature");
		return;
	}
}

void GHRenderDeviceDX12::flushGPU(void)
{
	mUploadCommandList->waitForCompletion();
	mComputeCommandList->waitForCompletion();
	for (int i = 0; i < NUM_SWAP_BUFFERS; ++i)
	{
		mFrameBackends[i].mCommandList->waitForCompletion();
	}
}

void GHRenderDeviceDX12::createDepthBuffer(void)
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = mDXDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDepthDescriptorHeap));
	if (FAILED(hr))
	{
		GHDebugMessage::outputString("Failed to create depth descriptor heap");
		return;
	}
	mDepthDescriptorHeap->SetName(L"Depth/Stencil Resource Heap");

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DEPTH_BUFFER_FORMAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	const GHPoint2i& screenSize = mWindow.getClientAreaSize();

	D3D12_HEAP_PROPERTIES heapProperties;
	GHDX12Helpers::createHeapProperties(heapProperties, D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC texDesc;
	GHDX12Helpers::createTexture2dDesc(texDesc, screenSize[0], screenSize[1]);
	texDesc.Format = DEPTH_BUFFER_FORMAT;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// msaa
	texDesc.SampleDesc.Count = MSAA_SAMPLE_COUNT;
	texDesc.SampleDesc.Quality = MSAA_SAMPLE_QUALITY;
	texDesc.MipLevels = 1;

	hr = mDXDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&mDepthBuffer)
	);
	if (FAILED(hr))
	{
		GHDebugMessage::outputString("Failed to create depth buffer");
		return;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DEPTH_BUFFER_FORMAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	mDXDevice->CreateDepthStencilView(mDepthBuffer.Get(), &depthStencilDesc, mDepthDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void GHRenderDeviceDX12::setClearColor(const GHPoint4& color)
{
	mClearColor = color;
}

void GHRenderDeviceDX12::clearBuffers(void)
{
	FLOAT clearColor[] = { mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3] };
	mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->ClearRenderTargetView(mActiveRTGroup.mRt0, clearColor, 0, nullptr);
	mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->ClearDepthStencilView(mActiveRTGroup.mDepth, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
