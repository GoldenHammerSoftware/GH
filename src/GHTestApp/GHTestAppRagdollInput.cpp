// Copyright 2010 Golden Hammer Software
#include "GHTestAppRagdollInput.h"
#include "GHEntity.h"
#include "GHInputState.h"
#include "GHEntityHashes.h"
#include "GHModel.h"
#include "GHPhysicsObject.h"
#include "GHKeyDef.h"

GHTestAppRagdollInput::GHTestAppRagdollInput(const GHInputState& inputState, GHEntity* entity)
: mInputState(inputState)
, mEntity(entity)
{
    
}

void GHTestAppRagdollInput::update(void)
{
    float sign = 1.0f;
    if (mInputState.getKeyState(0, GHKeyDef::KEY_SHIFT))
    {
        sign = -1.f;
    }
    
    const float impulseAmount = 2.f;
    if (mInputState.checkKeyChange(0, 'q', true))
    {
        applyImpulse(GHPoint3(sign*impulseAmount, 0, 0));
    }
    else if(mInputState.checkKeyChange(0, 'j', true))
    {
        applyImpulse(GHPoint3(0, 5.f*impulseAmount, 0));
    }
    else if(mInputState.checkKeyChange(0, 'k', true))
    {
        applyImpulse(GHPoint3(0, 0, sign*impulseAmount));
    }
}

void GHTestAppRagdollInput::applyImpulse(const GHPoint3& impulse)
{
    GHModel* model = mEntity->mProperties.getProperty(GHEntityHashes::MODEL);
    model->getPhysicsObject()->applyImpulse(impulse, GHPoint3(0,.1f,0));
}

void GHTestAppRagdollInput::onActivate(void)
{
    
}

void GHTestAppRagdollInput::onDeactivate(void)
{
    
}
