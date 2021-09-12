// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

class GHInputState;

// util class to keep track of how a pointer moved.
// will tell you how much a particular pointer moved this frame.
class GHPointerChangeTracker
{
public:
    GHPointerChangeTracker(int pointerIndex, const GHInputState& inputState);

    void updatePointerChange(GHPoint2& ret);
    void clear(void);

private:
    int mPointerIndex;
    const GHInputState& mInputState;
    bool mPointerWasActive;
    GHPoint2 mLastPointer;
};
