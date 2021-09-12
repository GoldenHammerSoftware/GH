// Copyright Golden Hammer Software
#pragma once

#include "GHRenderDeviceDX11.h"

class GHDX11SwapChainCreator;
class GHScreenInfo;

// A dx11 render device that manages the swap chain creation
class GHRenderDeviceDX11Native : public GHRenderDeviceDX11
{
public:
	GHRenderDeviceDX11Native(const int& graphicsQuality, GHDX11SwapChainCreator& swapChainCreator,
		const GHScreenInfo& screenInfo, GHMessageHandler& eventMgr, unsigned int desiredMSAA);

	virtual void endFrame(void);
	virtual void handleGraphicsQualityChange(void);
	virtual GHTexture* resolveBackbuffer(void);

protected:
	virtual void createDeviceResources();
    virtual void createWindowSizeDependentResources();

private:
	int convertDipsToPixels(float dips);

private:
	GHDX11SwapChainCreator& mSwapChainCreator;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> mSwapChain;
	const GHScreenInfo& mScreenInfo;
	unsigned int mDesiredMSAA;
};
