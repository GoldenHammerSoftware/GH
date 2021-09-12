// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include <list>
#include <vector>

class GHTimeCalculator;
class GHThreadFactory;
class GHThreadSleeper;

// a class to enforce a limited framerate by using sleep
class GHFramerateSmoother : public GHController
{
public:
    GHFramerateSmoother(const GHThreadFactory& threadFactory, const GHTimeCalculator& timeCalc);
    ~GHFramerateSmoother(void);
    
    virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void) {}

private:
    void changeExpectedSpeed(void);

private:
    const GHTimeCalculator& mTimeCalc;
    GHThreadSleeper* mThreadSleeper;
    float mMinUpdateTime;
    float mLastUpdateTime;
    float mLastFrameLength;
    
    std::list<int> mUpdateHistory;

	// list of acceptable update speeds from fastest to slowest.
	std::vector<float> mUpdateSpeeds;
	// current index into update speeds
	unsigned int mUpdateSpeedIdx;
};
