// Copyright Golden Hammer Software
#include "GHUtils/GHStateChangeMessageListener.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHStateMachine.h"

GHStateChangeMessageListener::GHStateChangeMessageListener(GHStateMachine& stateMachine,
                                                           const GHIdentifier& enumProp,
                                                           const GHIdentifier& idProp,
                                                           bool argIsState)
: mStateMachine(stateMachine)
, mEnumProp(enumProp)
, mIdProp(idProp)
, mArgIsState(argIsState)
{
}

void GHStateChangeMessageListener::handleMessage(const GHMessage& message)
{
    if (mArgIsState)
    {
        mStateMachine.setState(mEnumProp);
    }
    else 
    {
        const GHPropertyContainer& props = message.getPayload();
        const GHProperty& stateProp = props.getProperty(mEnumProp);
        if (stateProp.isValid())
        {
            int stateId = stateProp.getVal<int>();
            mStateMachine.setState(stateId);
        }
        else
        {
            const GHProperty& stateIdProp = props.getProperty(mIdProp);
            const GHIdentifier* stateIdPtr = (const GHIdentifier*)stateIdProp;
            if (stateIdPtr)
            {
                mStateMachine.setState(*stateIdPtr);
            }
        }
    }
}
