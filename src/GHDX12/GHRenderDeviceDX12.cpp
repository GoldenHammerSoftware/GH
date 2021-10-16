#include "GHRenderDeviceDX12.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX12Helpers.h"
#include "GHWin32/GHWin32Window.h"
#include "GHDX12Fence.h"
#include "Render/GHCamera.h"

#define DEBUG_DX12 1

static void createCommandListAndAllocator(Microsoft::WRL::ComPtr<ID3D12Device2>& device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& allocator)
{
	HRESULT allocRes = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&allocator);
	if (allocRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create d3d12 command allocator");
	}

	HRESULT listRes = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
	if (allocRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create d3d12 command list");
	}
	commandList->Close();
}

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
	const GHPoint2i& size = mWindow.getClientAreaSize();
	mDXSwapChain = GHDX12Helpers::createSwapChain(mWindow.getHWND(), mDXCommandQueue, size[0], size[1], NUM_SWAP_BUFFERS);
	const int numDescriptors = 256;
	mDXDescriptorHeap = GHDX12Helpers::createDescriptorHeap(mDXDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, numDescriptors);

	for (uint32_t frameId = 0; frameId < NUM_SWAP_BUFFERS; ++frameId)
	{
		GHDX12Helpers::createBackBuffer(mDXDevice, mDXSwapChain, mDXDescriptorHeap, mFrameBackends[frameId].mBackBuffer, frameId);

		createCommandListAndAllocator(mDXDevice, mFrameBackends[frameId].mDXCommandList, mFrameBackends[frameId].mDXCommandAllocator);

		mFrameBackends[frameId].mFence = new GHDX12Fence(mDXDevice);
	}
	createCommandListAndAllocator(mDXDevice, mDXUploadCommandList, mDXUploadCommandAllocator);
}

GHRenderDeviceDX12::~GHRenderDeviceDX12(void)
{
	if (mDXSwapChain) mDXSwapChain->SetFullscreenState(false, NULL);
	for (size_t frameId = 0; frameId < NUM_SWAP_BUFFERS; ++frameId)
	{
		if (mFrameBackends[frameId].mDXCommandList) mFrameBackends[frameId].mDXCommandList->Release();
		if (mFrameBackends[frameId].mBackBuffer) mFrameBackends[frameId].mBackBuffer->Release();
		if (mFrameBackends[frameId].mDXCommandAllocator) mFrameBackends[frameId].mDXCommandAllocator->Release();
	}
	if (mDXDescriptorHeap) mDXDescriptorHeap->Release();
	if (mDXSwapChain) mDXSwapChain->Release();
	if (mDXCommandQueue) mDXCommandQueue->Release();
	if (mDXDevice) mDXDevice->Release();
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
	mFrameBackends[mCurrBackend].mFence->waitForFence(mFrameBackends[mCurrBackend].mFenceWaitVal);

	mFrameBackends[mCurrBackend].mDXCommandAllocator->Reset();
	mFrameBackends[mCurrBackend].mDXCommandList->Reset(mFrameBackends[mCurrBackend].mDXCommandAllocator.Get(), nullptr);

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mFrameBackends[mCurrBackend].mBackBuffer.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	mFrameBackends[mCurrBackend].mDXCommandList->ResourceBarrier(1, &barrier);

	auto rtvDescriptorSize = mDXDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mDXDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += rtvDescriptorSize * mCurrBackend;
	mFrameBackends[mCurrBackend].mDXCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, NULL);

	FLOAT clearColor[] = { 0.1f, 0.6f, 0.1f, 1.0f };
	mFrameBackends[mCurrBackend].mDXCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

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
	mFrameBackends[mCurrBackend].mDXCommandList->ResourceBarrier(1, &barrier);

	mFrameBackends[mCurrBackend].mDXCommandList->Close();
	ID3D12CommandList* ppCommandLists[1];
	ppCommandLists[0] = mFrameBackends[mCurrBackend].mDXCommandList.Get();
	mDXCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	// possible todo: apply DXGI_PRESENT_ALLOW_TEARING or !vsync
	mDXSwapChain->Present(1, 0);

	mFrameBackends[mCurrBackend].mFenceWaitVal = mFrameBackends[mCurrBackend].mFence->addToCommandQueue(mDXCommandQueue);
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

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GHRenderDeviceDX12::beginUploadCommandList(void)
{
	mDXUploadCommandList->Reset(mDXUploadCommandAllocator.Get(), nullptr);
	return mDXUploadCommandList;
}

void GHRenderDeviceDX12::endUploadCommandList(void)
{
	mDXUploadCommandList->Close();
	ID3D12CommandList* ppCommandLists[] = { mDXUploadCommandList.Get() };
	mDXCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	// todo: add a fence to check during begin frame or maybe in beginUploadCommandList.
}
