// Copyright Golden Hammer Software
#include "GHWin32TimeCalculator.h"

GHWin32TimeCalculator::GHWin32TimeCalculator(void)
{
	mStartQPC.HighPart = 0;
	mStartQPC.LowPart = 0;
	mStartQPC.QuadPart = 0;
	QueryPerformanceFrequency(&mQueryPerformanceFrequency);
	// grab the initial time.
	QueryPerformanceCounter(&mStartQPC);
}

float GHWin32TimeCalculator::calcTime(void) const
{
	// find the current raw time val.
	LARGE_INTEGER tick;
	QueryPerformanceCounter(&tick);

	// subtract it from the start time to stay within a nicer range.
	long long currTime = (tick.QuadPart - mStartQPC.QuadPart);
	float fcurrTime = (float)
		((long double)(currTime / (long double)mQueryPerformanceFrequency.QuadPart));

	return fcurrTime;
}
