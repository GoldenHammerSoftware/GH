#pragma once

#include "Render/GHRenderServices.h"
#include "GHWin32/GHWin32FileFinder.h"

class GHWin32Window;

class GHRenderServicesDX12 : public GHRenderServices
{
public:
	GHRenderServicesDX12(GHSystemServices& systemServices, GHWin32Window& window);

	virtual void initAppShard(GHAppShard& appShard) override;

private:
	GHWin32Window& mWindow;
	GHWin32FileFinder mFileFinder;
};

