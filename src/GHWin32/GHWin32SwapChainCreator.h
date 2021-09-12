// Copyright Golden Hammer Software
#pragma once

#include "GHDX11SwapChainCreator.h"

class GHWin32Window;

class GHWin32SwapChainCreator : public GHDX11SwapChainCreator
{
public:
	GHWin32SwapChainCreator(const GHWin32Window& window, bool allowDXFullscreen);

	virtual Microsoft::WRL::ComPtr<IDXGISwapChain1> createSwapChain(IDXGIFactory2* factory,
		ID3D11Device1* device,
		const DXGI_SWAP_CHAIN_DESC1& description);
	virtual void swapChainUpdated(Microsoft::WRL::ComPtr<IDXGISwapChain1>) ;

private:
	const GHWin32Window& mWindow;
	bool mAllowDXFullscreen;
};
