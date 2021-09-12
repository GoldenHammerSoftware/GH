// Copyright Golden Hammer Software
#include "GHTimeVal.h"
#include "GHTimeCalculator.h"

GHTimeVal::GHTimeVal(void)
:	mTimePassed(0.0f)
,	mLastTime(0.0f)
,	mTime(0.0f)
{
}

void GHTimeVal::newFrame(float newTime)
{
	mLastTime = mTime;
	setTime(newTime);
	calcTimePassed();
}

void GHTimeVal::calcTimePassed(void) 
{ 
	if (!mLastTime) return;
	mTimePassed = mTime - mLastTime; 
	// don't let the clock run backwards! (iphone os bug?)
	if (mTimePassed < 0) mTimePassed = 0;
	// don't let super frames throw us into outer space either.
    const float sMaxTimestep = 0.1f;
	if (mTimePassed > sMaxTimestep) mTimePassed = sMaxTimestep;
}

void GHTimeVal::clearTimePassed(const GHTimeCalculator& timeCalculator)
{
	mLastTime = mTime;
	float normalizedTime = timeCalculator.calcTime();
	setTime(normalizedTime);
	mLastTime = mTime;
	calcTimePassed();
}
