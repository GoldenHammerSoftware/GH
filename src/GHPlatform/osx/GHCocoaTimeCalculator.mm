// Copyright 2010 Golden Hammer Software
#include "GHCocoaTimeCalculator.h"

GHCocoaTimeCalculator::GHCocoaTimeCalculator(void)
{
	mStartupTime = [[NSDate date] timeIntervalSinceReferenceDate];
}

float GHCocoaTimeCalculator::calcTime(void) const
{
	NSDate* date = [[NSDate alloc] init];
	double doubleSecs = [date timeIntervalSinceReferenceDate];	
	return (float)(doubleSecs - mStartupTime);
}

