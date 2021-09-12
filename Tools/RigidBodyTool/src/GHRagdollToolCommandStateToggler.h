// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"

class GHStateMachine;
class GHInputState;

class GHRagdollToolCommandStateToggler : public GHController
{
public:
    GHRagdollToolCommandStateToggler(GHStateMachine& stateMachine,
                                     const GHInputState& inputState);
    
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    virtual void update(void);
    
private:
    GHStateMachine& mStateMachine;
    const GHInputState& mInputState;
    bool mInCommandState;
};
