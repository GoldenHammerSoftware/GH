// Copyright Golden Hammer Software
#include "GHRenderDeviceFactoryDX11Native.h"
#include "GHRenderDeviceDX11Native.h"
#include "GHDX11SwapChainCreator.h"
#include "GHHMDRenderDeviceNull.h"

GHRenderDeviceFactoryDX11Native::GHRenderDeviceFactoryDX11Native(GHDX11SwapChainCreator& swapChainCreator,
																 GHMessageHandler& eventMgr, unsigned int desiredMSAA)
: mSwapChainCreator(swapChainCreator)
, mEventMgr(eventMgr)
, mDesiredMSAA(desiredMSAA)
{
}

GHRenderDeviceFactoryDX11Native::~GHRenderDeviceFactoryDX11Native(void)
{
	
}

GHRenderDeviceDX11* GHRenderDeviceFactoryDX11Native::createDevice(const int& graphicsQuality,
																  const GHScreenInfo& screenInfo)
{
	GHRenderDeviceDX11* ret = new GHRenderDeviceDX11Native(graphicsQuality, mSwapChainCreator, screenInfo, mEventMgr, mDesiredMSAA);
	return ret;
}

