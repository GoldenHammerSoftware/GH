// Copyright Golden Hammer Software
#pragma once

#include "Base/GHWindow.h"

class GHNullWindow : public GHWindow
{
public:
	virtual void resize(int width, int height) override {}
	virtual void setFullscreen(bool isFullscreen) override {}
};
