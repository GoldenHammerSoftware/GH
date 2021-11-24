// Copyright Golden Hammer Software
#pragma once

#include "Base/GHMouseCapturer.h"

class GHWin32Window;

class GHWin32MouseCapturer : public GHMouseCapturer
{
public:
	GHWin32MouseCapturer(GHWin32Window& window);

	virtual void captureMouse(void);
	virtual void releaseMouse(void);
	virtual void setMousePos(float x, float y);

	bool isCaptured(void) { return mIsCaptured; }

private:
	GHWin32Window& mWindow;
	bool mIsCaptured;
};
