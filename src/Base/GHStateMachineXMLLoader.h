// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifierMap.h"

class GHXMLObjFactory;
class GHStateMachine;
class GHStringIdFactory;

// <stateMachine>
//  <transitions stateId="ES_ACTIVE ES_RUNNING ES_DEAD" state="0 1 2" stateEnum="BE_SELECTED">
//      <addModelTransition/>
//  </transitions>
// </stateMachine>

// transition id types:
//  stateId - a GHIdentifier
//  state - a pure number
//  stateEnum - a lookup of string id to enum value.

class GHStateMachineXMLLoader : public GHXMLObjLoader
{
public:
    GHStateMachineXMLLoader(GHXMLObjFactory& objFactory,
                            const GHStringIdFactory& hashTable,
                            const GHIdentifierMap<int>& enumStore);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
private:
    GHXMLObjFactory& mObjFactory;
    const GHStringIdFactory& mIdFactory;
    const GHIdentifierMap<int>& mEnumStore;
};
