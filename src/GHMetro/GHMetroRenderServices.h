// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRenderServicesDX11.h"

class GHMetroTextureFinder;
class GHDX11SwapChainCreator;

class GHMetroRenderServices : public GHRenderServicesDX11
{
public:
	GHMetroRenderServices(GHSystemServices& systemServices,
						 GHRenderDeviceFactoryDX11* deviceFactory,
						 GHDX11SwapChainCreator* swapChainCreator,
						 const GHPoint2i& screenSize);
	~GHMetroRenderServices(void);

	virtual void initAppShard(GHAppShard& appShard);

private:
	GHMetroTextureFinder* mTextureFinder;
};
