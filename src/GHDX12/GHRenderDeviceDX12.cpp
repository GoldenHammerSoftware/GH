#include "GHRenderDeviceDX12.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX12Helpers.h"
#include "GHWin32/GHWin32Window.h"
#include "GHDX12Fence.h"
#include "Render/GHCamera.h"

#define DEBUG_DX12 1

GHRenderDeviceDX12::GHRenderDeviceDX12(GHWin32Window& window)
	: mWindow(window)
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
	}
	mUploadCommandList = new GHDX12CommandList(mDXDevice, mDXCommandQueue);

	mScissorRect.left = 0;
	mScissorRect.right = screenSize[0];
	mScissorRect.top = 0;
	mScissorRect.bottom = screenSize[1];

	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = screenSize[0];
	mViewport.Height = screenSize[1];
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
	delete mUploadCommandList;
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

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mFrameBackends[mCurrBackend].mBackBuffer.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->ResourceBarrier(1, &barrier);

	auto rtvDescriptorSize = mDXDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mDXDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += rtvDescriptorSize * mCurrBackend;
	mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, NULL);

	static FLOAT clearColor[] = { 0.1f, 0.0f, 0.1f, 1.0f };
	clearColor[1] += 0.01;
	if (clearColor[1] > 1) clearColor[1] = 0;
	mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	getRenderCommandList()->SetGraphicsRootSignature(mGraphicsRootSignature.Get());
	getRenderCommandList()->RSSetViewports(1, &mViewport); 
	getRenderCommandList()->RSSetScissorRects(1, &mScissorRect); 

	return true;
}

void GHRenderDeviceDX12::endFrame(void)
{
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mFrameBackends[mCurrBackend].mBackBuffer.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mFrameBackends[mCurrBackend].mCommandList->getDXCommandList()->ResourceBarrier(1, &barrier);

	mFrameBackends[mCurrBackend].mCommandList->endAndSubmit();

	// possible todo: apply DXGI_PRESENT_ALLOW_TEARING or !vsync
	mDXSwapChain->Present(1, 0);
}

void GHRenderDeviceDX12::beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers)
{
	// todo
}

void GHRenderDeviceDX12::endRenderPass(void)
{
	// todo
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
	return mUploadCommandList->getDXCommandList();
}

void GHRenderDeviceDX12::endUploadCommandList(void)
{
	mUploadCommandList->endAndSubmit();
	// wait for it to finish here to make sure everything is available.
	// if this is a bottleneck we can figure out something else.
	mUploadCommandList->waitForCompletion();
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
	D3D12_ROOT_PARAMETER params[3];
	desc.NumParameters = 3;
	desc.pParameters = &params[0];

	params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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

	const size_t maxTextures = 16;

	D3D12_DESCRIPTOR_RANGE srvDescRange;
	srvDescRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescRange.NumDescriptors = maxTextures;
	srvDescRange.BaseShaderRegister = 0;
	srvDescRange.RegisterSpace = 0;
	srvDescRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	params[2].DescriptorTable.NumDescriptorRanges = 1;
	params[2].DescriptorTable.pDescriptorRanges = &srvDescRange;

	desc.NumStaticSamplers = maxTextures;
	// todo: support different wrap modes etc.
	// we should probably canonize the types of samplers that are supported and use those in the shaders
	//  and remove the option of having the texture specify these things.
	D3D12_STATIC_SAMPLER_DESC samplers[maxTextures];
	for (int i = 0; i < maxTextures; ++i)
	{
		samplers[i].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		samplers[i].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplers[i].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplers[i].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplers[i].MipLODBias = 0;
		samplers[i].MaxAnisotropy = 0;
		samplers[i].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplers[i].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		samplers[i].MinLOD = 0.0f;
		samplers[i].MaxLOD = D3D12_FLOAT32_MAX;
		samplers[i].ShaderRegister = i;
		samplers[i].RegisterSpace = 0;
		samplers[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	}
	desc.pStaticSamplers = &samplers[0];

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
	for (int i = 0; i < NUM_SWAP_BUFFERS; ++i)
	{
		mFrameBackends[i].mCommandList->waitForCompletion();
	}
}
