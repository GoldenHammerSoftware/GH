// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHMath/GHPoint.h"

class GHGUIWidget;
class GHTimeVal;

// a controller to oscillate the size of a gui element while active
class GHGUISizeOscillater : public GHController
{
public:
	GHGUISizeOscillater(GHGUIWidget& widget, const GHTimeVal& time, float changeMax, float changeSpeed);

	virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

private:
	GHGUIWidget& mWidget;
	const GHTimeVal& mTime;

	// the max + difference from base size.
	float mChangeMax;
	// a multiplier to time for the oscillation.
	float mChangeSpeed;
	// the time where we started the oscillation.
	float mStartTime;
	// the unoscillated size of the widget.
	GHPoint2 mBaseSize;
};
