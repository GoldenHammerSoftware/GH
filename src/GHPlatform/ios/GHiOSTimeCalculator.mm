// Copyright 2010 Golden Hammer Software
#include "GHiOSTimeCalculator.h"

GHiOSTimeCalculator::GHiOSTimeCalculator(void)
{
    mStartupTime = [[NSDate date] timeIntervalSinceReferenceDate];
}

float GHiOSTimeCalculator::calcTime(void) const
{
    NSDate* date = [[NSDate alloc] init];
	double doubleSecs = [date timeIntervalSinceReferenceDate];	
	return (float)(doubleSecs - mStartupTime);
}
