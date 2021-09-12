// Copyright Golden Hammer Software
#pragma once

class GHRenderDeviceDX11;
class GHScreenInfo;

// we need different devices on win8 vs wp8
class GHRenderDeviceFactoryDX11
{
public:
	virtual ~GHRenderDeviceFactoryDX11(void) {}

	virtual GHRenderDeviceDX11* createDevice(const int& graphicsQuality, const GHScreenInfo& screenInfo) = 0;
};

