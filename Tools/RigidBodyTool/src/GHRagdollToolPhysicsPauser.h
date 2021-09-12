// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"

class GHInputState;
class GHControllerMgr;
class GHPhysicsSim;
class GHRigidBodyTool;
class GHPropertyContainer;

class GHRagdollToolPhysicsPauser : public GHController
{
public:
    GHRagdollToolPhysicsPauser(const GHInputState& inputState,
                               GHControllerMgr& controllerMgr,
                               GHPhysicsSim& physicsSim,
                               GHRigidBodyTool& tool,
                               GHPropertyContainer& properties);
    
    
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    virtual void update(void);
    
private:
    const GHInputState& mInputState;
    GHControllerMgr& mControllerMgr;
    GHPhysicsSim& mPhysicsSim;
    GHRigidBodyTool& mRigidBodyTool;
    GHPropertyContainer& mProperties;
    
    int mFrameCount = 0;
    bool mIsPaused;
    bool mDebugDrawIsOn;
    bool mIsStepping;
};
