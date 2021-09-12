// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"
#include "GHPoint.h"

class GHPhysicsObject;
class GHInputState;
class GHPropertyContainer;

class GHRagdollToolObjectPusher : public GHController
{
public:
    GHRagdollToolObjectPusher(const GHInputState& inputState,
                              GHPropertyContainer& props,
                              GHPhysicsObject*& target);
    
    virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    
private:
    void applyImpulse(const GHPoint3& impulse);
    
private:
    const GHInputState& mInputState;
    GHPropertyContainer& mProps;
    GHPhysicsObject*& mTarget;
    float mImpulseAmount;
    
    const static int BUFLEN = 64;
    char mMsgBuf[BUFLEN];
};
