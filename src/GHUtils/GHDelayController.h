// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHTimeVal;
class GHTransition;

// a controller to do something after a time has passed.
// deactivate cancels the something.
// we take ownership of the transition.
class GHDelayController : public GHController
{
public:
    GHDelayController(const GHTimeVal& time, float delay,
                      GHTransition* transition);
    ~GHDelayController(void);
    
    virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void) {}
    
private:
    const GHTimeVal& mTime;
    float mDelay;
    GHTransition* mTransition;

    float mTimePassed;
    bool mTriggered;
};
