// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include <list>

class GHTimeVal;
class GHTimeCalculator;

// A controller to update a time val.
// If this controller is deactivated and reactivated, the time val will
// act as if time stood still, and continue on from where it left off.
// pretends that the time passed is the average actual time passed for the past few frames
class GHTimeUpdaterAveraging : public GHController
{
public:
	GHTimeUpdaterAveraging(const GHTimeCalculator& timeCalc, GHTimeVal& timeVal);

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

	std::list<float> mTimePassedHistory;
	float mLastActualTime;
};
