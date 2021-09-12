// Copyright 2010 Golden Hammer Software
#include "GHMetroXAMLSwapChainCreator.h"
#include "GHPlatform/GHDebugMessage.h"
#include <windows.ui.xaml.media.dxinterop.h>

GHMetroXAMLSwapChainCreator::GHMetroXAMLSwapChainCreator(Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ swapChainBG)
	: mSwapChainBG(swapChainBG)
{
}

Microsoft::WRL::ComPtr<IDXGISwapChain1> GHMetroXAMLSwapChainCreator::createSwapChain(
																		IDXGIFactory2* factory, 
																		ID3D11Device1* device,
																		const DXGI_SWAP_CHAIN_DESC1& description)
{
	Microsoft::WRL::ComPtr<IDXGISwapChain1> ret = 0;
	HRESULT res = factory->CreateSwapChainForComposition(
									device,
									&description,
									nullptr, //allow on all displays
									&ret);


	if (FAILED(res)) GHDebugMessage::outputString("Failed to create swap chain");


	Microsoft::WRL::ComPtr<ISwapChainBackgroundPanelNative> panelNative;
	res = reinterpret_cast<IUnknown*>(mSwapChainBG)->QueryInterface(IID_PPV_ARGS(&panelNative));
	if (FAILED(res)) GHDebugMessage::outputString("Failed to get pointer to ISwapChainBackgroundPanelNative.");

	if (panelNative) 
	{
		res = panelNative->SetSwapChain(ret.Get());
		if (FAILED(res)) GHDebugMessage::outputString("Failed to set swap chain on the swap chain background panel.");
	}

	return ret;
}

void GHMetroXAMLSwapChainCreator::swapChainUpdated(Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain)
{
	Microsoft::WRL::ComPtr<ISwapChainBackgroundPanelNative> panelNative;
	HRESULT res = reinterpret_cast<IUnknown*>(mSwapChainBG)->QueryInterface(IID_PPV_ARGS(&panelNative));
	if (FAILED(res)) GHDebugMessage::outputString("Failed to get pointer to ISwapChainBackgroundPanelNative.");

	if (panelNative) 
	{
		panelNative->SetSwapChain(nullptr);
		res = panelNative->SetSwapChain(swapChain.Get());
		if (FAILED(res)) GHDebugMessage::outputString("Failed to set swap chain on the swap chain background panel.");
	}
	else
	{
		GHDebugMessage::outputString("Failed to convert swap chain to native swap chain.");
	}
}

