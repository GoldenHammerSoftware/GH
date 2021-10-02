#include "GHRenderDeviceDX12.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX12Helpers.h"
#include "GHWin32/GHWin32Window.h"

GHRenderDeviceDX12::GHRenderDeviceDX12(GHWin32Window& window)
	: mWindow(window)
{
	Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter = findDX12Adapter();
	HRESULT deviceRes = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDXDevice));
	if (deviceRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create d3d12 device");
	}
	mDXCommandQueue = createCommandQueue(mDXDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
	const GHPoint2i& size = mWindow.getClientAreaSize();
	mDXSwapChain = createSwapChain(mWindow.getHWND(), mDXCommandQueue, size[0], size[1], NUM_SWAP_BUFFERS);
	const int numDescriptors = 256;
	mDXDescriptorHeap = createDescriptorHeap(mDXDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, numDescriptors);
	createBackBuffers(mDXDevice, mDXSwapChain, mDXDescriptorHeap, mBackBuffers, NUM_SWAP_BUFFERS);

	HRESULT allocRes = mDXDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&mDXCommandAllocator);
	if (allocRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create d3d12 command allocator");
	}
	HRESULT listRes = mDXDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mDXCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mDXCommandList));
	if (allocRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create d3d12 command list");
	}

}

GHRenderDeviceDX12::~GHRenderDeviceDX12(void)
{
}

void GHRenderDeviceDX12::reinit(void)
{
	// todo.
}

bool GHRenderDeviceDX12::beginFrame(void)
{
	// todo
	return true;
}

void GHRenderDeviceDX12::endFrame(void)
{
	// todo
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
	// todo
}

void GHRenderDeviceDX12::createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
	GHViewInfo::ViewTransforms& deviceTransforms,
	const GHCamera& camera, bool isRenderToTexture) const
{
	// todo
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
