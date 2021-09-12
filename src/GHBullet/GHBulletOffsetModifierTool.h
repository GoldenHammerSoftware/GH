// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHMath/GHPoint.h"
#include <vector>
#include "GHMath/GHTransform.h"

class GHInputState;
class GHBulletPhysicsObject;

//Debug use only:
// This class takes user input and modifies the offset of a
// rigid body in real time, printing out the current values.
class GHBulletOffsetModifierTool : public GHController
{
public:
    GHBulletOffsetModifierTool(const GHInputState& inputState,
                               GHBulletPhysicsObject& physicsObject);
    
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    virtual void update(void);
    
    GHBulletOffsetModifierTool* clone(std::vector<GHBulletPhysicsObject*>& objectList) const;
    
private:
    void pushChange(void);

private:
    const GHInputState& mInputState;
    GHBulletPhysicsObject& mPhysicsObject;
    
    GHTransform mThisFrameOffset;

    GHPoint3 mCurrentOffset;
    GHPoint3 mCurrentRotationOffset;
};
