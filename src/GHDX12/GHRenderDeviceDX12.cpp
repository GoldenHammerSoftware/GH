#include "GHRenderDeviceDX12.h"
#include "GHPlatform/GHDebugMessage.h"

static Microsoft::WRL::ComPtr<IDXGIAdapter4> findDX12Adapter(void)
{
	Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;

	UINT createFactoryFlags = 0;
	//createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

	HRESULT res = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
	if (res != S_OK)
	{
		GHDebugMessage::outputString("Failed to create dxgi factory");
		return 0;
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter1;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter4;
	SIZE_T maxDedicatedVideoMemory = 0;
	// find the adapter with the largest amount of video memory and pick that one.
	for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
		dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

		if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
			SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
				D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
			dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
		{
			maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
			dxgiAdapter1.As(&dxgiAdapter4);
		}
	}

	return dxgiAdapter4;
}

static Microsoft::WRL::ComPtr<ID3D12CommandQueue> createCommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
{
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue));
	return d3d12CommandQueue;
}

GHRenderDeviceDX12::GHRenderDeviceDX12(void)
{
	Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter = findDX12Adapter();
	HRESULT deviceRes = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDXDevice));
	if (deviceRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create d3d12 device");
	}
	mDXCommandQueue = createCommandQueue(mDXDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
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
