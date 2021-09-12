// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHUtils/GHStateMachine.h"

// A transition to change state on a specified machine on activate
// ignores deactivate
class GHStateChangeTransition : public GHTransition
{
public:
    GHStateChangeTransition(GHStateMachine& stateMachine,
                            GHStateMachine::StateId state);
    
    virtual void activate(void);
	virtual void deactivate(void) {}
    
private:
    GHStateMachine& mStateMachine;
    GHStateMachine::StateId mState;
};
