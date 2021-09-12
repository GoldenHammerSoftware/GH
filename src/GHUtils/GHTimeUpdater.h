// Copyright Golden Hammer Software
#pragma once

#include "GHController.h"

class GHTimeVal;
class GHTimeCalculator;

// A controller to update a time val.
// If this controller is deactivated and reactivated, the time val will
// act as if time stood still, and continue on from where it left off.
class GHTimeUpdater : public GHController
{
public:
    GHTimeUpdater(const GHTimeCalculator& timeCalc, GHTimeVal& timeVal);

    virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

private:
    const GHTimeCalculator& mTimeCalc;
    GHTimeVal& mTimeVal;
    
    // current time = what timeCalc thinks - mStartTime.
    float mStartTime;
    // deactivate time is used to increment mStartTime after a pause.
    float mDeactivateTime;
};
