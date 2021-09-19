#pragma once

#include "Render/GHRenderServices.h"

class GHWin32Window;

class GHRenderServicesDX12 : public GHRenderServices
{
public:
	GHRenderServicesDX12(GHSystemServices& systemServices, GHWin32Window& window);

private:
	GHWin32Window& mWindow;
};

