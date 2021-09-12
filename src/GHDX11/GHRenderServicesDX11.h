// Copyright Golden Hammer Software
#pragma once

#include "GHRenderServices.h"

class GHShaderDX11;
class GHRenderStateMgrDX11;
class GHRenderDeviceFactoryDX11;
class GHRenderDeviceDX11;
class GHDX11SwapChainCreator;

class GHRenderServicesDX11 : public GHRenderServices
{
public:
    GHRenderServicesDX11(GHSystemServices& systemServices,
						 GHRenderDeviceFactoryDX11* deviceFactory,
						 GHDX11SwapChainCreator* swapChainCreator,
						 const GHPoint2i& screenSize,
						 int initialGraphicsQuality);
	~GHRenderServicesDX11(void);

	virtual void initAppShard(GHAppShard& appShard);

	GHRenderDeviceDX11* getDeviceDX11(void) const;
	GHDX11SwapChainCreator* getSwapChainCreator(void) { return mSwapChainCreator; }

protected:
	// ownership of the device factory for deletion
	GHRenderDeviceFactoryDX11* mDeviceFactory;
	GHDX11SwapChainCreator* mSwapChainCreator{ 0 };
	// assuming sending things to the gpu has to be serial:
	//  the current VS being used.
	GHShaderDX11* mActiveVS;
	// Class to prevent duplicate state changes.
	GHRenderStateMgrDX11* mStateMgr;
};
