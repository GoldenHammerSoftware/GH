// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolSavedRagdollXMLLoader.h"
#include "GHXMLNodeDeepCopier.h"
#include "GHRagdollGenerator.h"
#include "GHEntityHashes.h"
#include "GHPropertyContainer.h"
#include "GHXMLNode.h"
#include "GHEntity.h"
#include "GHModel.h"
#include "GHRagdollToolNode.h"

GHRagdollToolSavedRagdollXMLLoader::GHRagdollToolSavedRagdollXMLLoader(const GHXMLObjFactory& objFatory,
                                                                       const GHIdentifierMap<int>& enumStore,
                                                                       const GHStringHashTable& hashtable,
                                                                       GHControllerMgr& controllerMgr,
                                                                       GHPhysicsSim& physicsSim,
                                                                       const GHXMLSerializer& xmlSerializer)
: mObjFactory(objFatory)
, mEnumStore(enumStore)
, mHashtable(hashtable)
, mControllerMgr(controllerMgr)
, mPhysicsSim(physicsSim)
, mXMLSerializer(xmlSerializer)
{
    
}

void* GHRagdollToolSavedRagdollXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHXMLNode* xmlCopy = GHXMLNodeDeepCopier::copy(node);
    
    GHEntity* targetEnt = (GHEntity*)extraData.getProperty(GHEntityHashes::P_ENTITY);
    GHModel* model = (GHModel*)targetEnt->mProperties.getProperty(GHEntityHashes::MODEL);
    
    const char* activeIDStr = node.getAttribute("activeID");
    GHIdentifier activeID = mHashtable.generateHash(activeIDStr ? activeIDStr : "ES_ACTIVE");

    
    GHRagdollGenerator* ret = new GHRagdollGenerator(mObjFactory,
                                                      mEnumStore,
                                                      targetEnt,
                                                      mPhysicsSim,
                                                      mControllerMgr,
                                                      activeID,
                                                      xmlCopy);
    
    loadRagdoll(*ret, model->getSkeleton(), xmlCopy);
    return ret;
}

void GHRagdollToolSavedRagdollXMLLoader::loadRagdoll(GHRagdollGenerator& ret, GHTransformNode* parentSkeleton, GHXMLNode* node) const
{
    GHXMLNode* bonesNode = node->getChild("bones", false);
    if (!bonesNode) { return; }
    
    GHXMLNode::NodeList& bones = bonesNode->getChildren();
    size_t numBones = bones.size();
    for (size_t i = 0; i < numBones; ++i)
    {
        GHXMLNode* bone = bones[i];
        loadPhysicsObject(ret, parentSkeleton, bone, (int)i);
    }
    
    GHXMLNode* constraintsNode = node->getChild("constraints", false);
    if (!constraintsNode) { return; }
    
    GHXMLNode::NodeList& constraints = constraintsNode->getChildren();
    size_t numConstraints = constraints.size();
    for (size_t i = 0; i < numConstraints; ++i)
    {
        GHXMLNode* constraint = constraints[i];
        loadConstraint(ret, constraint);
    }
    
    ret.buildPregeneratedRagdoll();
}

void GHRagdollToolSavedRagdollXMLLoader::loadPhysicsObject(GHRagdollGenerator& ret, GHTransformNode* parentSkeleton, GHXMLNode* node, int index) const
{
    GHRagdollToolNode* toolNode = new GHRagdollToolNode(mObjFactory, mEnumStore, parentSkeleton, ret, node, index);
    ret.setNode(mHashtable.generateHash(node->getName()), toolNode);
}

void GHRagdollToolSavedRagdollXMLLoader::loadConstraint(GHRagdollGenerator& ret, GHXMLNode* node) const
{
    GHIdentifier bodyBID = mHashtable.generateHash(node->getAttribute("bodyB"));
    GHRagdollToolNode* toolNode = ret.getNode(bodyBID);
    if (toolNode) {
        toolNode->setConstraintNode(node);
    }
}
