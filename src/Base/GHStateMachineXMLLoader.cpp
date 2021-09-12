// Copyright Golden Hammer Software
#include "GHStateMachineXMLLoader.h"
#include "GHXMLObjFactory.h"
#include "GHUtils/GHStateMachine.h"
#include "GHXMLNode.h"
#include "GHString/GHHashListParser.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHPropertyStacker.h"
#include "GHBaseIdentifiers.h"

GHStateMachineXMLLoader::GHStateMachineXMLLoader(GHXMLObjFactory& objFactory,
                                                 const GHStringIdFactory& hashTable,
                                                 const GHIdentifierMap<int>& enumStore)
: mObjFactory(objFactory)
, mIdFactory(hashTable)
, mEnumStore(enumStore)
{
}

void* GHStateMachineXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHStateMachine* ret = new GHStateMachine;
    populate(ret, node, extraData);
    return ret;
}

void GHStateMachineXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHStateMachine& ret = *((GHStateMachine*)obj);
    
    GHPropertyStacker stateMachineStacker(extraData, GHBaseIdentifiers::STATEMACHINE, obj);

    std::vector<GHIdentifier> statesIds;
    std::vector<int> states;
    for (size_t currStateId = 0; currStateId < node.getChildren().size(); ++currStateId)
    {
        const GHXMLNode* currNode = node.getChildren()[currStateId];
        const char* stateIdString = currNode->getAttribute("stateId");
        statesIds.resize(0);
        if (stateIdString) {
            GHHashListParser::parseHashList(stateIdString, mIdFactory, statesIds);
        }
        states.resize(0);
        currNode->readAttrIntList("state", states);
        
        const char* stateEnumString = currNode->getAttribute("stateEnum");
        if (stateEnumString) {
            GHHashListParser::parseHashEnumList(stateEnumString, mEnumStore, states);
        }
        
        if (!statesIds.size() && !states.size()) {
            GHDebugMessage::outputString("No state specified for transitions.");
            continue;
        }
        
        for (size_t currTransId = 0; currTransId < currNode->getChildren().size(); ++currTransId)
        {
            GHTransition* currTrans = (GHTransition*)mObjFactory.load(*currNode->getChildren()[currTransId], &extraData);
            if (currTrans)
            {
                for (size_t i = 0; i < statesIds.size(); ++i)
                {
                    ret.addTransition(statesIds[i], currTrans);
                }
                for (size_t i = 0; i < states.size(); ++i) 
                {
                    ret.addTransition(states[i], currTrans);
                }
            }
        }
    }
}
