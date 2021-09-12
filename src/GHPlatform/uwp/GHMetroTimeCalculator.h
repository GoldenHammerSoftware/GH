// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHTimeCalculator.h"
#include <Windows.h> // not super happy about including this here for LARGE_INTEGER.

class GHMetroTimeCalculator : public GHTimeCalculator
{
public:
	GHMetroTimeCalculator(void);
	virtual float calcTime(void) const;

private:
	// The QPC time when the app started.
	// Without this value we start overflowing getting into funky
	//  ranges after a few days without rebooting the computer.
	// If that happens, the framerate gets choked to about 10fps.
	LARGE_INTEGER mStartQPC;
	// The frequency val of QPC.
	LARGE_INTEGER mQueryPerformanceFrequency;
};
