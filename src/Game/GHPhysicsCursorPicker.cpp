// Copyright Golden Hammer Software
#include "GHPhysicsCursorPicker.h"
#include "GHInputState.h"
#include "GHPhysicsSim.h"

GHPhysicsCursorPicker::GHPhysicsCursorPicker(const GHInputState& inputState,
                                             const GHViewInfo& viewInfo,
                                             GHPhysicsSim& physicsSim)
: mInputState(inputState)
, mUnprojector(viewInfo)
, mPhysicsSim(physicsSim)
, mDist(500) //arbitrary
{
    
}

GHPhysicsObject* GHPhysicsCursorPicker::pickObject(int cursorIndex)
{
    const GHPoint2& screenPos = mInputState.getPointerPosition(cursorIndex);
    GHPoint3 origin, dir;
    mUnprojector.unproject(screenPos, origin, dir);
    dir *= mDist;
    dir += origin;
    
    GHPhysicsCollisionData::GHPhysicsCollisionObject hitObj;
    if (mPhysicsSim.castRay(origin, dir, ~0, hitObj))
    {
        return hitObj.mObject;
    }
    return 0;
}
