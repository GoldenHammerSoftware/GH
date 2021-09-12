// Copyright Golden Hammer Software
#include "GHEntityXMLLoader.h"
#include "GHEntity.h"
#include "GHPropertyContainerXMLLoader.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLObjFactory.h"
#include "GHStateMachineXMLLoader.h"
#include "GHXMLNode.h"
#include "GHEntityHashes.h"
#include "GHUtils/GHPropertyStacker.h"
#include "GHModel.h"
#include "GHUtils/GHUtilsIdentifiers.h"

GHEntityXMLLoader::GHEntityXMLLoader(GHXMLObjFactory& objFactory)
: mObjFactory(objFactory)
, mNextEntId(1)
{
}

void* GHEntityXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHEntity* ret = new GHEntity;
    populate(ret, node, extraData);
    return ret;
}

void GHEntityXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHEntity* ret = (GHEntity*)obj;
    GHPropertyStacker entStack(extraData, GHEntityHashes::P_ENTITY, ret);
    GHPropertyStacker propStack(extraData, GHUtilsIdentifiers::PROPS, &ret->mProperties);
    
    GHPropertyContainerXMLLoader* propLoader = (GHPropertyContainerXMLLoader*)mObjFactory.getLoader("properties");
    if (!propLoader) {
        GHDebugMessage::outputString("Could not find properties loader for entity");
    }
    GHStateMachineXMLLoader* smLoader = (GHStateMachineXMLLoader*)mObjFactory.getLoader("stateMachine");
    if (!smLoader) {
        GHDebugMessage::outputString("Could not find state machine loader for entity");
    }
    
    const GHXMLNode* propsNode = node.getChild("properties", false);
    if (propsNode) propLoader->populate(&ret->mProperties, *propsNode, extraData);
    const GHXMLNode* smNode = node.getChild("stateMachine", false);
    if (smNode) smLoader->populate(&ret->mStateMachine, *smNode, extraData);
    
    assignGUID(ret->mProperties, extraData);
    
    GHModel* model = (GHModel*)ret->mProperties.getProperty(GHEntityHashes::MODEL);
    if (model) {
        model->setEntityData(&ret->mProperties);
    }
}

void GHEntityXMLLoader::assignGUID(GHPropertyContainer& entProps, const GHPropertyContainer& extraData) const
{
    int entId = (int)extraData.getProperty(GHEntityHashes::P_ENTID);
    if (entId == 0)
    {
        entId = mNextEntId;
        mNextEntId++;
    }
    entProps.setProperty(GHEntityHashes::P_ENTID, entId);
}

