// Copyright Golden Hammer Software
#include "GHWin32MouseCapturer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/win32/GHWin32Include.h"
#include "GHWin32Window.h"

GHWin32MouseCapturer::GHWin32MouseCapturer(GHWin32Window& window)
: mWindow(window)
, mIsCaptured(false)
{
}

void GHWin32MouseCapturer::captureMouse(void)
{
	if (mIsCaptured) return;
	ShowCursor(false);
	mIsCaptured = true;
}

void GHWin32MouseCapturer::releaseMouse(void)
{
	if (!mIsCaptured) return;
	ShowCursor(true);
	mIsCaptured = false;
}

void GHWin32MouseCapturer::setMousePos(float x, float y)
{
	const GHPoint2i& windowPos = mWindow.getPosition();
	const GHPoint2i& windowSize = mWindow.getClientAreaSize();
	SetCursorPos(windowPos[0] + int(windowSize[0] * x),
		windowPos[1] + int(windowSize[1] * y));
}

