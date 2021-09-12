// Copyright Golden Hammer Software
#pragma once

#include "GHWin32Include.h"
#include "GHPlatform/GHTimeCalculator.h"

class GHWin32TimeCalculator : public GHTimeCalculator
{
public:
	GHWin32TimeCalculator(void);

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
