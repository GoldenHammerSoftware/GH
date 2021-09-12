// Copyright Golden Hammer Software
#include "GHTimeUpdaterAveraging.h"
#include "GHPlatform/GHTimeCalculator.h"
#include "GHPlatform/GHTimeVal.h"

GHTimeUpdaterAveraging::GHTimeUpdaterAveraging(const GHTimeCalculator& timeCalc, GHTimeVal& timeVal)
: mTimeCalc(timeCalc)
, mTimeVal(timeVal)
, mStartTime(timeCalc.calcTime())
, mDeactivateTime(timeCalc.calcTime())
, mLastActualTime(0)
{
}

void GHTimeUpdaterAveraging::update(void)
{
	float currTime = mTimeCalc.calcTime() - mStartTime;

	float timePassed = currTime - mLastActualTime;
	mLastActualTime = currTime;
	mTimePassedHistory.push_back(timePassed);
	while (mTimePassedHistory.size() > 10) {
		mTimePassedHistory.pop_front();
	}
	float avePassed = 0;
	for (std::list<float>::const_iterator iter = mTimePassedHistory.begin(); iter != mTimePassedHistory.end(); ++iter)
	{
		avePassed += *iter;
	}
	avePassed /= mTimePassedHistory.size();

	mTimeVal.newFrame(mTimeVal.getTime() + avePassed);
}

void GHTimeUpdaterAveraging::onActivate(void)
{
	float timePaused = mTimeCalc.calcTime() - mDeactivateTime;
	mStartTime += timePaused;
}

void GHTimeUpdaterAveraging::onDeactivate(void)
{
	mDeactivateTime = mTimeCalc.calcTime();
	// zero out the time passed since it will not be getting updated.
	float currTime = mTimeVal.getTime();
	mTimeVal.newFrame(currTime);
}
