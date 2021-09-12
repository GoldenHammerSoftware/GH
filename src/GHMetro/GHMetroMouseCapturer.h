// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMouseCapturer.h"

class GHMetroMouseCapturer : public GHMouseCapturer
{
public:
	GHMetroMouseCapturer(void);
	~GHMetroMouseCapturer(void);

	virtual void captureMouse(void);
	virtual void releaseMouse(void);
	virtual void setMousePos(float x, float y);

	bool isCaptured(void) const { return mIsCaptured; }

private:
	Windows::UI::Core::CoreCursor^ mReplacedCursor;
	bool mIsCaptured;
};
