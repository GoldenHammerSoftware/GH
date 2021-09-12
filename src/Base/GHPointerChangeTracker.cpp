// Copyright Golden Hammer Software
#include "GHPointerChangeTracker.h"
#include "GHInputState.h"

GHPointerChangeTracker::GHPointerChangeTracker(int pointerIndex, const GHInputState& inputState)
: mPointerIndex(pointerIndex)
, mInputState(inputState)
, mPointerWasActive(false)
, mLastPointer(0,0)
{
}

void GHPointerChangeTracker::updatePointerChange(GHPoint2& ret)
{
    ret.setCoords(0,0);
    if (!mPointerWasActive)
    {
        if (mInputState.getPointerActive(mPointerIndex))
        {
            mPointerWasActive = true;
            mLastPointer = mInputState.getPointerPosition(mPointerIndex);
        }
        return;
    }
    mPointerWasActive = mInputState.getPointerActive(mPointerIndex);
    ret = mInputState.getPointerPosition(mPointerIndex);
    ret -= mLastPointer;
    mLastPointer = mInputState.getPointerPosition(mPointerIndex);
}

void GHPointerChangeTracker::clear(void)
{
    mPointerWasActive = false;
}
