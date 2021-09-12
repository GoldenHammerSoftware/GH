// Copyright Golden Hammer Software
#include "GHSkeletonDebugDrawerXMLLoader.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHEntityHashes.h"
#include "GHModel.h"
#include "GHSkeletonDebugDrawer.h"
#include "GHEntity.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHControllerTransition.h"
#include "GHString/GHStringIdFactory.h"

GHSkeletonDebugDrawerXMLLoader::GHSkeletonDebugDrawerXMLLoader(GHControllerMgr& controllerMgr, GHStringIdFactory& hashTable)
: mControllerMgr(controllerMgr)
, mIdFactory(hashTable)
{
    
}

void* GHSkeletonDebugDrawerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHEntity* entity = (GHEntity*)extraData.getProperty(GHEntityHashes::P_ENTITY);
    if (!entity)
    {
        GHDebugMessage::outputString("Error creating GHSkeletonDebugDrawer: no entity in extraData");
        return 0;
    }
    
    GHModel* model = (GHModel*)entity->mProperties.getProperty(GHEntityHashes::MODEL);
    if (!model || !model->getSkeleton())
    {
        GHDebugMessage::outputString("Error creating GHSkeletonDebugDrawer: no model with skeleton on the entity");
        return 0;
    }
    
    GHPoint3 color(0,0,1);
    node.readAttrFloatArr("color", color.getArr(), 3);
    
    float lineLength = .02f;
    node.readAttrFloat("lineLength", lineLength);
    
    GHTransformNode* targetNode = model->getSkeleton();
    assert(targetNode);
    const char* targetNodeName = node.getAttribute("targetNode");
    if (targetNodeName) {
        GHIdentifier targId = mIdFactory.generateHash(targetNodeName);
        targetNode = targetNode->findChild(targId);
        if (!targetNode) {
            GHDebugMessage::outputString("failed to find target node");
            targetNode = model->getSkeleton();
        }
    }
    return new GHControllerTransition(mControllerMgr, new GHSkeletonDebugDrawer(targetNode, lineLength, color));
}

