// Copyright 2010 Golden Hammer Software
#include "GHCocoaThreadSleeper.h"

void GHCocoaThreadSleeper::sleep(int ms) 
{
	float sleepTimeInSeconds = (float)ms / 1000.0f;
	[NSThread sleepForTimeInterval:sleepTimeInSeconds];
}
