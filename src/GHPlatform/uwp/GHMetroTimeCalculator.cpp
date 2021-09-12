// Copyright 2010 Golden Hammer Software
#include "GHMetroTimeCalculator.h"
#include <windows.h>

GHMetroTimeCalculator::GHMetroTimeCalculator(void)
{
	mStartQPC.HighPart = 0;
	mStartQPC.LowPart = 0;
	mStartQPC.QuadPart = 0;
	QueryPerformanceFrequency(&mQueryPerformanceFrequency);
	// grab the initial time.
	QueryPerformanceCounter(&mStartQPC);
}

float GHMetroTimeCalculator::calcTime(void) const
{
	// find the current raw time val.
	LARGE_INTEGER tick;
	QueryPerformanceCounter(&tick);

	// subtract it from the start time to stay within a nicer range.
	long long currTime = (tick.QuadPart-mStartQPC.QuadPart);
	float fcurrTime = (float)
		((long double)(currTime/(long double)mQueryPerformanceFrequency.QuadPart));

	return fcurrTime;
}

