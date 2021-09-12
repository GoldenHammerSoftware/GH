// Copyright Golden Hammer Software
#include "GHUtils/GHStateChangeTransition.h"

GHStateChangeTransition::GHStateChangeTransition(GHStateMachine& stateMachine,
                                                 GHStateMachine::StateId state)
: mStateMachine(stateMachine)
, mState(state)
{
}

void GHStateChangeTransition::activate(void)
{
    mStateMachine.setState(mState);
}
