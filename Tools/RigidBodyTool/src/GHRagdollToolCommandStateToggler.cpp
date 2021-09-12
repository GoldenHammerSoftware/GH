// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolCommandStateToggler.h"
#include "GHStateMachine.h"
#include "GHInputState.h"
#include "GHKeyDef.h"
#include "GHRagdollToolProperties.h"

GHRagdollToolCommandStateToggler::GHRagdollToolCommandStateToggler(GHStateMachine& stateMachine,
                                                                   const GHInputState& inputState)
: mStateMachine(stateMachine)
, mInputState(inputState)
, mInCommandState(false)
{
    
}

void GHRagdollToolCommandStateToggler::onActivate(void)
{
    
}

void GHRagdollToolCommandStateToggler::onDeactivate(void)
{
    
}

void GHRagdollToolCommandStateToggler::update(void)
{
    if (mInputState.checkKeyChange(0, GHKeyDef::KEY_ENTER, true))
    {
        if (mInCommandState)
        {
            mStateMachine.setState(GHRagdollToolProperties::MAININPUTSTATE);
            mInCommandState = false;
        }
        else
        {
            mStateMachine.setState(GHRagdollToolProperties::COMMANDINPUTSTATE);
            mInCommandState = true;
        }
    }
}
