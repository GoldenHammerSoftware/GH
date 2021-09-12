// Copyright Golden Hammer Software
#include "GHStateChangeTransitionXMLLoader.h"
#include "GHUtils/GHStateChangeTransition.h"
#include "GHBaseIdentifiers.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHXMLNode.h"
#include "GHString/GHStringIdFactory.h"

GHStateChangeTransitionXMLLoader::GHStateChangeTransitionXMLLoader(const GHStringIdFactory& hashtable,
                                                                   const GHIdentifierMap<int>& enumStore)
: mIdFactory(hashtable)
, mEnumStore(enumStore)
{
    
}

void* GHStateChangeTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHStateMachine* stateMachine = (GHStateMachine*)extraData.getProperty(GHBaseIdentifiers::STATEMACHINE);
    if (!stateMachine) {
        GHDebugMessage::outputString("Error, trying to load a stateChangeTransition with no GHBaseIdentifiers::STATEMACHINE in extraData! Returning 0");
        return 0;
    }
    
    int stateEnum;
    GHIdentifier stateId;
    GHStateMachine::StateId state;
    if (node.readAttrEnum("stateEnum", stateEnum, mEnumStore))
    {
        state = stateEnum;
    }
    else if (node.readAttrIdentifier("stateId", stateId, mIdFactory))
    {
        state = stateId;
    }
    else {
        GHDebugMessage::outputString("Error, trying to load a stateChangeTransition with no stateEnum property and no stateId! Set one of these. Returning 0.");
        return 0;
    }
    
    return new GHStateChangeTransition(*stateMachine, state);
}
