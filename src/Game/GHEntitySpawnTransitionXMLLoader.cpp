// Copyright Golden Hammer Software
#include "GHEntitySpawnTransitionXMLLoader.h"
#include "GHString/GHStringIdFactory.h"
#include "GHXMLObjFactory.h"
#include "GHXMLNode.h"
#include "GHMath/GHPoint.h"
#include "GHEntity.h"
#include "GHEntitySpawnTransition.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHEntityHashes.h"
#include "GHModel.h"

GHEntitySpawnTransitionXMLLoader::GHEntitySpawnTransitionXMLLoader(const GHStringIdFactory& hashTable,
                                                                   const GHXMLObjFactory& objFactory)
: mIdFactory(hashTable)
, mObjFactory(objFactory)
{
}

void* GHEntitySpawnTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    if (!node.getChildren().size()) {
        GHDebugMessage::outputString("No entity specified for spawner");
        return 0;
    }
    GHEntity* entity = (GHEntity*)mObjFactory.load(*node.getChildren()[0]);
    if (!entity) {
        GHDebugMessage::outputString("Could not load entity for spawner");
    }
    

    GHPoint3 spawnPoint(0,0,0);
    node.readAttrFloatArr("pos", spawnPoint.getArr(), 3);
    GHTransform spawnTrans;
    spawnTrans.becomeIdentity();
    spawnTrans.setTranslate(spawnPoint);
    
    const char* activeStr = node.getAttribute("activate");
    GHIdentifier activeId = mIdFactory.generateHash(activeStr);
    
    const char* parentNodeName = node.getAttribute("parentNode");
    GHEntity* parentEnt = extraData.getProperty(GHEntityHashes::P_ENTITY);
    GHTransformNode* parentNode = 0;
    if (parentNodeName && parentEnt)
    {
        GHModel* parentModel = parentEnt->mProperties.getProperty(GHEntityHashes::MODEL);
        if (parentModel && parentModel->getSkeleton())
        {
            GHIdentifier parentNodeId = mIdFactory.generateHash(parentNodeName);
            parentNode = parentModel->getSkeleton()->findChild(parentNodeId);
            if (!parentNode) {
                GHDebugMessage::outputString("Failed to find parent node for entity spawn");
                parentNode = parentModel->getSkeleton();
            }
        }
    }
    
    GHEntitySpawnTransition* ret = new GHEntitySpawnTransition(entity, activeId, parentNode, spawnTrans);
    return ret;
}
