// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHDX11SwapChainCreator.h"

//Creates a swap chain that is compatible with pure Direct3D projects.
class GHMetroXAMLSwapChainCreator : public GHDX11SwapChainCreator
{
public:
	GHMetroXAMLSwapChainCreator(Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ swapChainBG);
	virtual Microsoft::WRL::ComPtr<IDXGISwapChain1> createSwapChain(IDXGIFactory2* factory, 
																	ID3D11Device1* device,
																	const DXGI_SWAP_CHAIN_DESC1& description);
	virtual void swapChainUpdated(Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain);

private:
	Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ mSwapChainBG;
};
