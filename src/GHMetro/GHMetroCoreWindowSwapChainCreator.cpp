// Copyright 2010 Golden Hammer Software
#include "GHMetroCoreWindowSwapChainCreator.h"
#include "GHPlatform/GHDebugMessage.h"

Microsoft::WRL::ComPtr<IDXGISwapChain1> GHMetroCoreWindowSwapChainCreator::createSwapChain(
																	IDXGIFactory2* factory, 
																	ID3D11Device1* device,
																	const DXGI_SWAP_CHAIN_DESC1& description)
{
	Microsoft::WRL::ComPtr<IDXGISwapChain1> ret = 0;

	Windows::UI::Core::CoreWindow^ p = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	assert(p);

	HRESULT res = factory->CreateSwapChainForCoreWindow(
										       device,
										       reinterpret_cast<IUnknown*>(p),
											   &description,
										       nullptr,    // allow on all displays
										       &ret
										       );

	if (FAILED(res)) GHDebugMessage::outputString("Failed to create swap chain");

	return ret;
}
