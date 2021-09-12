// Copyright 2010 Golden Hammer Software
#include "GHiOSThreadSleeper.h"

void GHiOSThreadSleeper::sleep(int ms) 
{
	float sleepTimeInSeconds = (float)ms / 1000.0f;
	[NSThread sleepForTimeInterval:sleepTimeInSeconds];
}
