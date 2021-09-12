// Copyright Golden Hammer Software
#pragma once
#include "GHUtils/GHTransition.h"

class GHMouseCapturer;

// captures the mouse when activated, releases on deactivate.
class GHMouseCapturerTransition : public GHTransition
{
public:
	GHMouseCapturerTransition(GHMouseCapturer& mouseCapturer);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	GHMouseCapturer& mMouseCapturer;
};
