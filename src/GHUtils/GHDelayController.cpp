// Copyright Golden Hammer Software
#include "GHUtils/GHDelayController.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHUtils/GHTransition.h"
#include <assert.h>

GHDelayController::GHDelayController(const GHTimeVal& time, float delay,
                                     GHTransition* transition)
: mTime(time)
, mDelay(delay)
, mTransition(transition)
, mTimePassed(0)
, mTriggered(false)
{
    assert(mTransition != 0);
}

GHDelayController::~GHDelayController(void)
{
    delete mTransition;
}

void GHDelayController::update(void)
{
    if (mTriggered) return;
    mTimePassed += mTime.getTimePassed();
    if (mTimePassed >= mDelay)
    {
        mTriggered = true;
        mTransition->activate();
    }
}

void GHDelayController::onActivate(void)
{
    mTriggered = false;
    mTimePassed = 0;
}
