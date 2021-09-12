// Copyright Golden Hammer Software
#pragma once

#include "GHDX11Include.h"

//An interface for creating a swap chain, branched
// based on the type of Windows 8 app.

class GHDX11SwapChainCreator
{
public:

	//Pass in desired MSAA. Will be changed to actual.
	bool createSwapChain(int width, int height, 
						 Microsoft::WRL::ComPtr<ID3D11Device1>& d3dDevice,
						 Microsoft::WRL::ComPtr<IDXGIDevice1>& dxgiDevice, Microsoft::WRL::ComPtr<IDXGIAdapter>& dxgiAdapter,
						 Microsoft::WRL::ComPtr<IDXGISwapChain1>& outSwapChain, unsigned int& inOutMSAA);

	virtual Microsoft::WRL::ComPtr<IDXGISwapChain1> createSwapChain(IDXGIFactory2* factory, 
																	ID3D11Device1* device,
																	const DXGI_SWAP_CHAIN_DESC1& description) = 0;
	virtual void swapChainUpdated(Microsoft::WRL::ComPtr<IDXGISwapChain1>) = 0;
};
