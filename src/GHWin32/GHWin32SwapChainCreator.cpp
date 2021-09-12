// Copyright Golden Hammer Software
#include "GHWin32SwapChainCreator.h"
#include "GHWin32Window.h"
#include "GHPlatform/GHDebugMessage.h"

GHWin32SwapChainCreator::GHWin32SwapChainCreator(const GHWin32Window& window, bool allowDXFullscreen)
: mWindow(window)
, mAllowDXFullscreen(allowDXFullscreen)
{
}

Microsoft::WRL::ComPtr<IDXGISwapChain1> GHWin32SwapChainCreator::createSwapChain(IDXGIFactory2* factory,
	ID3D11Device1* device,
	const DXGI_SWAP_CHAIN_DESC1& description)
{
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

	HRESULT createRes = factory->CreateSwapChainForHwnd(device,
		mWindow.getHWND(),
		&description,
		NULL, // todo: fullscreen
		NULL, // restrict to output?
		swapChain.GetAddressOf()
		);

	if (createRes == S_OK) {
		GHDebugMessage::outputString("Created DX11 Swap Chain");
	}
	else if (createRes == E_OUTOFMEMORY) {
		GHDebugMessage::outputString("Not enough memory for DX11 Swap Chain");
	}
	else if (createRes == DXGI_ERROR_INVALID_CALL) {
		GHDebugMessage::outputString("Invalid call to DX11 Swap Chain creation");
	}
	else {
		GHDebugMessage::outputString("Unknown DX11 Swap Chain creation error");
	}

	// alt-enter causes issues with file dialog.
	if (!mAllowDXFullscreen)
	{
		factory->MakeWindowAssociation(mWindow.getHWND(), DXGI_MWA_NO_ALT_ENTER);
	}

	return swapChain;
}

void GHWin32SwapChainCreator::swapChainUpdated(Microsoft::WRL::ComPtr<IDXGISwapChain1>)
{
	// todo?
	GHDebugMessage::outputString("GHWin32SwapChainCreator::swapChainUpdated");
}
