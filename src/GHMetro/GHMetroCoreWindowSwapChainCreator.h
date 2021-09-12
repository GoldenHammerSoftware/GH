// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHDX11SwapChainCreator.h"

//Creates a swap chain that is compatible with pure Direct3D projects.
class GHMetroCoreWindowSwapChainCreator : public GHDX11SwapChainCreator
{
public:
	virtual Microsoft::WRL::ComPtr<IDXGISwapChain1> createSwapChain(IDXGIFactory2* factory, 
																	ID3D11Device1* device,
																	const DXGI_SWAP_CHAIN_DESC1& description);
	virtual void swapChainUpdated(Microsoft::WRL::ComPtr<IDXGISwapChain1>) {}
};
