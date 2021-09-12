// Copyright Golden Hammer Software
#include "GHFramerateSmoother.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHTimeCalculator.h"
#include "GHPlatform/GHThreadSleeper.h"
#include "GHPlatform/GHDebugMessage.h"
#include <math.h>

GHFramerateSmoother::GHFramerateSmoother(const GHThreadFactory& threadFactory,
                                       const GHTimeCalculator& timeCalc)
: mTimeCalc(timeCalc)
, mThreadSleeper(threadFactory.createSleeper())
, mMinUpdateTime(0)
, mLastUpdateTime(0)
, mLastFrameLength(0)
{
	//mUpdateSpeeds.push_back(0.0f);
	//mUpdateSpeeds.push_back(1.0f/60.0f);
	//mUpdateSpeeds.push_back(1.0f/30.0f);
	mUpdateSpeeds.push_back(1.0f/20.0f);
	mUpdateSpeedIdx = 0;
	mMinUpdateTime = mUpdateSpeeds[mUpdateSpeedIdx];
}

GHFramerateSmoother::~GHFramerateSmoother(void)
{
    delete mThreadSleeper;
}

void GHFramerateSmoother::update(void)
{
    changeExpectedSpeed();

    float currTime = mTimeCalc.calcTime();
    float timePassed = currTime - mLastUpdateTime;

    float sleepLen = 0;
    if (timePassed < mMinUpdateTime && mMinUpdateTime)
    {
        sleepLen = (mMinUpdateTime-timePassed)*1000.0f;
        mThreadSleeper->sleep((int)sleepLen);
    }
    float postSleepTime = mTimeCalc.calcTime();
    float frameLength = postSleepTime - mLastUpdateTime;

    mLastFrameLength = frameLength;
    mLastUpdateTime = postSleepTime;
}

void GHFramerateSmoother::onActivate(void)
{
    mLastUpdateTime = mTimeCalc.calcTime();
}

void GHFramerateSmoother::changeExpectedSpeed(void)
{
    const int sHistoryLen = 100;
    
    float currTime = mTimeCalc.calcTime();
    float nonSleepLen = currTime - mLastUpdateTime;
    
    // rule out super long loading frames.
    if (nonSleepLen > 0.6f) {
        return;
    }
    
	int fasterUpdateIdx = mUpdateSpeedIdx - 1;
	if (fasterUpdateIdx < 0) fasterUpdateIdx = 0;
	float fasterUpdateSpeed = mUpdateSpeeds[fasterUpdateIdx];

    // slow the framerate on instantaneous drops.
	if (::fabs(nonSleepLen - mMinUpdateTime) < 0.01)
	{
		// close enough frame.
		mUpdateHistory.push_back(0);
	}
    else if (nonSleepLen > mMinUpdateTime) {
		// slow frame.
        mUpdateHistory.push_back(1);
    }
    // if we get enough fast frames in a row increase the framerate.
    else if (nonSleepLen < fasterUpdateSpeed + 0.003)
    {
		// fast frame.
        mUpdateHistory.push_back(-1);
    }
    else {
		// average frame.
        mUpdateHistory.push_back(0);
    }
    if (mUpdateHistory.size() > sHistoryLen) {
        mUpdateHistory.pop_front();
    }
    
    int numSlow = 0;
    int numFast = 0;
    int numEqual = 0;
    std::list<int>::iterator histIter;
    for (histIter = mUpdateHistory.begin(); histIter != mUpdateHistory.end(); ++histIter)
    {
        if (*histIter > 0) numSlow++;
        else if (*histIter < 0) numFast++;
        else numEqual++;
    }
    
    if (mUpdateHistory.size() == sHistoryLen)
    {
        if (numSlow >= 2)
        {
			if (mUpdateSpeedIdx < mUpdateSpeeds.size() - 1) {
				mUpdateSpeedIdx++;
				mMinUpdateTime = mUpdateSpeeds[mUpdateSpeedIdx];
				GHDebugMessage::outputString("Changed update speed to %f", mMinUpdateTime);
			}
            mUpdateHistory.clear();
        }
        else if (mUpdateHistory.size() == sHistoryLen &&
                 numSlow == 0 &&
                 numEqual == 0)
        {
			if (mUpdateSpeedIdx > 0) {
				mUpdateSpeedIdx--;
				mMinUpdateTime = mUpdateSpeeds[mUpdateSpeedIdx];
				GHDebugMessage::outputString("Changed update speed to %f", mMinUpdateTime);
			}
            mUpdateHistory.clear();
        }
    }
}
