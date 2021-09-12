// Copyright Golden Hammer Software
#pragma once

#include "GHRenderDeviceFactoryDX11.h"

class GHDX11SwapChainCreator;
class GHMessageHandler;

// for creating a dx11 render device that manages its own backbuffer
class GHRenderDeviceFactoryDX11Native : public GHRenderDeviceFactoryDX11
{
public:
	GHRenderDeviceFactoryDX11Native(GHDX11SwapChainCreator& swapChainCreator, GHMessageHandler& eventMgr, unsigned int desiredMSAA);
	~GHRenderDeviceFactoryDX11Native(void);

	virtual GHRenderDeviceDX11* createDevice(const int& graphicsQuality, const GHScreenInfo& screenInfo);

private:
	GHDX11SwapChainCreator& mSwapChainCreator;
	GHMessageHandler& mEventMgr;
	unsigned int mDesiredMSAA;
};
