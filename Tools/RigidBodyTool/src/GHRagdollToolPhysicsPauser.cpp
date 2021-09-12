// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolPhysicsPauser.h"
#include "GHInputState.h"
#include "GHControllerMgr.h"
#include "GHPhysicsSim.h"
#include "GHRigidBodyTool.h"
#include "GHPropertyContainer.h"
#include "GHRagdollToolProperties.h"

GHRagdollToolPhysicsPauser::GHRagdollToolPhysicsPauser(const GHInputState& inputState,
                                                       GHControllerMgr& controllerMgr,
                                                       GHPhysicsSim& physicsSim,
                                                       GHRigidBodyTool& rigidBodyTool,
                                                       GHPropertyContainer& properties)
: mInputState(inputState)
, mControllerMgr(controllerMgr)
, mPhysicsSim(physicsSim)
, mRigidBodyTool(rigidBodyTool)
, mProperties(properties)
, mIsPaused(false)
, mDebugDrawIsOn(false)
, mIsStepping(false)
{
    
}

void GHRagdollToolPhysicsPauser::onActivate(void)
{
    
}

void GHRagdollToolPhysicsPauser::onDeactivate(void)
{
    
}

void GHRagdollToolPhysicsPauser::update(void)
{
    if (mInputState.checkKeyChange(0, '`', true))
    {
        if (mIsPaused)
        {
            mPhysicsSim.unpause();
            mIsPaused = false;
        }
        else
        {
            mPhysicsSim.pause();
            mIsPaused = true;
        }
    }
    
    if (mInputState.checkKeyChange(0, 'r', true))
    {
        mRigidBodyTool.resetRagdoll();
        if (!mIsPaused)
        {
            mPhysicsSim.pause();
        }
        mProperties.setProperty(GHRagdollToolProperties::SELECTEDBONE, "Nothing");
    }
    
    if (mInputState.checkKeyChange(0, '1', true))
    {
        if (mDebugDrawIsOn)
        {
            mPhysicsSim.enableDebugDraw(false);
            mDebugDrawIsOn = false;
        }
        else
        {
            mPhysicsSim.enableDebugDraw(true);
            mDebugDrawIsOn = true;
        }
    }
    
    if (mInputState.checkKeyChange(0, '3', true))
    {
        mPhysicsSim.unpause();
        mIsStepping = true;
        mIsPaused = true;
    }
    else if (mIsStepping)
    {
        mPhysicsSim.pause();
        mIsPaused = true;
        mIsStepping = false;
    }
}
