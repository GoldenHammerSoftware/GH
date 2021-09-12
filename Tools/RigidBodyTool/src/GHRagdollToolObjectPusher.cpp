// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolObjectPusher.h"
#include "GHInputState.h"
#include "GHBulletPhysicsObject.h"
#include "GHKeyDef.h"
#include "GHPhysicsObject.h"
#include "GHPropertyContainer.h"
#include "GHRagdollToolProperties.h"

GHRagdollToolObjectPusher::GHRagdollToolObjectPusher(const GHInputState& inputState,
                                                     GHPropertyContainer& props,
                                                     GHPhysicsObject*& target)
: mInputState(inputState)
, mProps(props)
, mTarget(target)
, mImpulseAmount(5.f)
{
    
}

void GHRagdollToolObjectPusher::update(void)
{
    if (!mTarget) { return; }
    
    float sign = 1.0f;
    if (mInputState.getKeyState(0, GHKeyDef::KEY_SHIFT))
    {
        sign = -1.f;
    }
    
    if (mInputState.checkKeyChange(0, 'q', true))
    {
        applyImpulse(GHPoint3(sign*mImpulseAmount, 0, 0));
    }
    else if(mInputState.checkKeyChange(0, 'j', true))
    {
        applyImpulse(GHPoint3(0, 5.f*mImpulseAmount, 0));
    }
    else if(mInputState.checkKeyChange(0, 'k', true))
    {
        applyImpulse(GHPoint3(0, 0, sign*mImpulseAmount));
    }
    else if (mInputState.checkKeyChange(0, ';', true)
             || mInputState.checkKeyChange(0, ':', true))
    {
        if (sign > 0) {
            mImpulseAmount += 1.f;
        }
        else {
            mImpulseAmount -= 1.f;
        }
        snprintf(mMsgBuf, BUFLEN, "Impulse: %f", mImpulseAmount);
        mProps.setProperty(GHRagdollToolProperties::COMMANDRESULT, mMsgBuf);
    }
    
}

void GHRagdollToolObjectPusher::applyImpulse(const GHPoint3& impulse)
{
    mTarget->applyImpulse(impulse, GHPoint3(0,.1f,0));
}

void GHRagdollToolObjectPusher::onActivate(void)
{
    
}

void GHRagdollToolObjectPusher::onDeactivate(void)
{
    
}