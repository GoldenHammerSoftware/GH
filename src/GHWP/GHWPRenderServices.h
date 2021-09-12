// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRenderServicesDX11.h"

class GHRenderDeviceFactory;
class GHMetroTextureFinder;

class GHWPRenderServices : public GHRenderServicesDX11
{
public:
	GHWPRenderServices(GHSystemServices& systemServices,
						 GHRenderDeviceFactoryDX11* deviceFactory,
						 const GHPoint2i& screenSize);
	~GHWPRenderServices(void);

	virtual void initAppShard(GHAppShard& appShard);

private:
	GHMetroTextureFinder* mTextureFinder;
};
