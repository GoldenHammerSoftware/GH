// Copyright Golden Hammer Software
#include "GHMouseCapturerTransition.h"
#include "GHMouseCapturer.h"

GHMouseCapturerTransition::GHMouseCapturerTransition(GHMouseCapturer& mouseCapturer)
: mMouseCapturer(mouseCapturer)
{
}

void GHMouseCapturerTransition::activate(void)
{
	mMouseCapturer.captureMouse();
}

void GHMouseCapturerTransition::deactivate(void)
{
	mMouseCapturer.releaseMouse();
}
