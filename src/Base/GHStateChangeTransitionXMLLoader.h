// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifierMap.h"

class GHStringIdFactory;

/*
 <stateChangeTransition stateId=GAMESTATE_IDENTIFIER>
 </stateChangeTransition>
 
 or
 
 <stateChangeTransition stateEnum=GAMESTATE_ENUMERATION>
 </stateChangeTransition>
 
 The state machine comes from GHBaseIdentifiers::STATEMACHINE on extraData.
 When activated, the transition will set the XML-specified state on the state machine.
 
 */
class GHStateChangeTransitionXMLLoader : public GHXMLObjLoader
{
public:
    GHStateChangeTransitionXMLLoader(const GHStringIdFactory& hashtable,
                                     const GHIdentifierMap<int>& enumStore);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHStringIdFactory& mIdFactory;
    const GHIdentifierMap<int>& mEnumStore;
};
