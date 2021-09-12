// Copyright Golden Hammer Software
#include "GHEntityModelPhysicsTransitionXMLLoader.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHModel.h"
#include "GHModelPhysicsTransition.h"
#include "GHPhysicsObject.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"

GHEntityModelPhysicsTransitionXMLLoader::GHEntityModelPhysicsTransitionXMLLoader(GHPhysicsSim& physicsSim,
                                                                                 const GHXMLObjFactory& objFactory)
: mPhysicsSim(physicsSim)
, mObjFactory(objFactory)
{
}

void* GHEntityModelPhysicsTransitionXMLLoader::create(const GHXMLNode& node,
                                                      GHPropertyContainer& extraData) const
{
    GHEntity* parentEnt = extraData.getProperty(GHEntityHashes::P_ENTITY);
    if (!parentEnt) {
        GHDebugMessage::outputString("EMPTransition loaded with no parent entity.");
        return 0;
    }
    GHModel* parentModel = parentEnt->mProperties.getProperty(GHEntityHashes::MODEL);
    if (!parentModel) {
        GHDebugMessage::outputString("EMPTransition called with no model.");
        return 0;
    }
    if (!parentModel->getPhysicsObject()) {
        GHDebugMessage::outputString("EMPTransition called with no physics object.");
        return 0;
    }
    
    GHPhysicsObject* physicsObject = 0;
    const GHXMLNode::NodeList& children = node.getChildren();
    if (children.size())
    {
        const GHXMLNode* physicsNode = children[0];
        GHProperty oldModelProp = extraData.getProperty(GHEntityHashes::MODEL);
        extraData.setProperty(GHEntityHashes::MODEL, parentModel);
        physicsObject = (GHPhysicsObject*)mObjFactory.load(*physicsNode, &extraData);
        extraData.setProperty(GHEntityHashes::MODEL, oldModelProp);
    }
    
    if (physicsObject) {
        physicsObject->setUserData(parentEnt);
    }
    else {
        parentModel->getPhysicsObject()->setUserData(parentEnt);
    }
    
    return new GHModelPhysicsTransition(*parentModel, mPhysicsSim, physicsObject);
}
