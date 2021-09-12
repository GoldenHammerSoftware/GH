// Copyright Golden Hammer Software
#include "GHFramerateMonitor.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHUtils/GHPropertyContainer.h"

GHFramerateMonitor::GHFramerateMonitor(const GHTimeVal& timeVal,
                                       GHPropertyContainer& props,
                                       const GHIdentifier& fpsProp,
                                       const GHIdentifier& msProp)
: mTimeVal(timeVal)
, mProps(props)
, mFPSProp(fpsProp)
, mMSProp(msProp)
, mCurrentHistory(0)
{
    for (int i = 0; i < HISTORY_LEN; ++i) {
		mTimeHistory[i] = 0;
	}
}

void GHFramerateMonitor::update(void)
{
	float currTime = mTimeVal.getTime();
    
	float oldestTime = mTimeHistory[mCurrentHistory];
    
	mTimeHistory[mCurrentHistory] = currTime;
	mCurrentHistory++;
	if (mCurrentHistory == HISTORY_LEN) mCurrentHistory = 0;
    
	float totTime = mTimeVal.getTime() - oldestTime;
	if (totTime <= 0.001) return;
	float msPerFrame = (totTime/(float)HISTORY_LEN);
	mProps.setProperty(mMSProp, msPerFrame*1000);
	mProps.setProperty(mFPSProp, 1.0f/msPerFrame);
}
