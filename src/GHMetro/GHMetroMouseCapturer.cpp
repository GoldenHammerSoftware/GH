// Copyright 2010 Golden Hammer Software
#include "GHMetroMouseCapturer.h"
#include "GHPlatform/GHDebugMessage.h"
#include <assert.h>

GHMetroMouseCapturer::GHMetroMouseCapturer(void) 
: mIsCaptured(false) 
, mReplacedCursor(nullptr)
{
}

GHMetroMouseCapturer::~GHMetroMouseCapturer(void)
{
	// make sure we don't exit with no mouse, just in case windows doesn't clean up properly.
	releaseMouse();
}

void GHMetroMouseCapturer::captureMouse(void)
{
	if (mIsCaptured) {
		// don't remove twice.
		return;
	}
	Windows::UI::Core::CoreWindow^ cw = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	if (!cw) return;
	mReplacedCursor = cw->PointerCursor;
	cw->PointerCursor = nullptr;
	mIsCaptured = true;
}

void GHMetroMouseCapturer::releaseMouse(void)
{
	if (!mIsCaptured) {
		return;
	}
	Windows::UI::Core::CoreWindow^ cw = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	if (!cw) return;
	assert(mReplacedCursor != nullptr);
	cw->PointerCursor = mReplacedCursor;
	mIsCaptured = false;
}

void GHMetroMouseCapturer::setMousePos(float x, float y)
{
	GHDebugMessage::outputString("SetMousePos unimplemented!");
}
