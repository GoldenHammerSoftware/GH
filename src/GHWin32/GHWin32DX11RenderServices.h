// Copyright Golden Hammer Software
#pragma once

#include "GHDX11/GHRenderServicesDX11.h"
#include "GHWindowsFileFinder.h"

class GHWin32DX11RenderServices : public GHRenderServicesDX11
{
public:
	GHWin32DX11RenderServices(GHSystemServices& systemServices,
							  GHRenderDeviceFactoryDX11* deviceFactory,
							  GHDX11SwapChainCreator* swapChainCreator,
							  const GHPoint2i& screenSize);
	~GHWin32DX11RenderServices(void);

	virtual void initAppShard(GHAppShard& appShard);

private:
	GHWindowsFileFinder* mTextureFinder;
};
