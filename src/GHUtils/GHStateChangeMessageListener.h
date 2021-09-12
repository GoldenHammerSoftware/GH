// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"
#include "GHString/GHIdentifier.h"

class GHStateMachine;

// A message listener for changing states of a specified state machine.
// if argIsState we use enumProp as target state
// else if enumProp is on the message we use getProp(enumProp) as target state
// else if idProp is on the message we use getProp(idProp) as target state
class GHStateChangeMessageListener : public GHMessageHandler
{
public:
    GHStateChangeMessageListener(GHStateMachine& stateMachine,
                                 const GHIdentifier& enumProp,
                                 const GHIdentifier& idProp,
                                 bool argIsState=false);
    
    virtual void handleMessage(const GHMessage& message);
    
private:
    GHStateMachine& mStateMachine;
    // identifier used for the id of the state to transition to.
    GHIdentifier mEnumProp;
    GHIdentifier mIdProp;
    bool mArgIsState;
};
