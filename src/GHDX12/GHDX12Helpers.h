#pragma once

// reference: https://www.3dgep.com/learning-directx-12-1/

#include "GHDX12Include.h"
#include "GHPlatform/GHDebugMessage.h"

Microsoft::WRL::ComPtr<IDXGIAdapter4> findDX12Adapter(void)
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

Microsoft::WRL::ComPtr<ID3D12CommandQueue> createCommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
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

bool checkTearingSupport(void)
{
	BOOL allowTearing = true;

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory4;
	if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
	{
		Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
		if (SUCCEEDED(factory4.As(&factory5)))
		{
			if (FAILED(factory5->CheckFeatureSupport(
				DXGI_FEATURE_PRESENT_ALLOW_TEARING,
				&allowTearing, sizeof(allowTearing))))
			{
				allowTearing = false;
			}
		}
	}

	return allowTearing;
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> createSwapChain(HWND hWnd,
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
	uint32_t width, uint32_t height, uint32_t bufferCount)

{
	Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain4;
	Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory4;

	UINT createFactoryFlags = 0;
	//createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

	HRESULT factoryRes = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4));
	if (factoryRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create dx12 swap chain factory");
		return 0;
	}

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = checkTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
	HRESULT swapRes = dxgiFactory4->CreateSwapChainForHwnd(
		commandQueue.Get(),
		hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1);
	if (swapRes != S_OK)
	{
		GHDebugMessage::outputString("Failed to create dx12 swap chain");
		return 0;
	}

	dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	swapChain1.As(&dxgiSwapChain4);

	return dxgiSwapChain4;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device2> device,
	D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;

	HRESULT res = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap));
	if (res != S_OK)
	{
		GHDebugMessage::outputString("Failed to create descriptor heap");
	}

	return descriptorHeap;
}

void createBackBuffers(Microsoft::WRL::ComPtr<ID3D12Device2> &device, Microsoft::WRL::ComPtr<IDXGISwapChain1> &swapChain, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> &descriptorHeap,  Microsoft::WRL::ComPtr<ID3D12Resource> *backBuffers, size_t numSwapBuffers)
{
	auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (int i = 0; i < numSwapBuffers; ++i)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
		HRESULT swapRes = swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
		if (swapRes != S_OK)
		{
			GHDebugMessage::outputString("Failed to create swap buffer");
		}

		device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
		backBuffers[i] = backBuffer;

		rtvHandle.ptr += rtvDescriptorSize;
	}
}
