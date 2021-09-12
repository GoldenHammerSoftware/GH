// Copyright Golden Hammer Software
#include "GHBulletRagdollXMLLoader.h"
#include "GHBulletPhysicsObject.h"
#include "GHBulletRagdollConstraint.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHEntityHashes.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHModel.h"
#include "GHBulletRagdoll.h"
#include "GHBulletBtInclude.h"
#include <algorithm>
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"
#include "GHXMLSerializer.h"
#include "GHString/GHStringIdFactory.h"
#include "GHBulletRagdollConstraintXMLLoader.h"
#include "GHBulletObjectXMLLoader.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHBulletOffsetModifierTool.h"

GHBulletRagdollXMLLoader::GHBulletRagdollXMLLoader(GHBulletObjectXMLLoader& bulletObjectLoader,
                                                   GHBulletPhysicsSim& physicsSim,
                                                   const GHXMLSerializer& xmlSerializer,
                                                   const GHStringIdFactory& hashTable,
                                                   GHControllerMgr& controllerMgr,
                                                   const GHInputState& inputState)
: mBulletObjectLoader(bulletObjectLoader)
, mPhysicsSim(physicsSim)
, mXMLSerializer(xmlSerializer)
, mIdFactory(hashTable)
, mControllerMgr(controllerMgr)
, mInputState(inputState)
{
    
}

static void print(const GHTransformNode* transformNode)
{
    GHPoint3 pos;
    const_cast<GHTransformNode*>(transformNode)->getTransform().getTranslate(pos);

    GHPoint3 diff(0,0,0);
    if (transformNode->getParent())
    {
        GHPoint3 parentPos;
        transformNode->getParent()->getTransform().getTranslate(parentPos);
        diff = pos;
        diff -= parentPos;
    }
    
    GHDebugMessage::outputString("%s parent=%s pos=%f %f %f diffFromParent=%f %f %f", transformNode->getId().getString(), transformNode->getParent() ? transformNode->getParent()->getId().getString() : "none", pos[0], pos[1], pos[2], diff[0], diff[1], diff[2]);
    for (size_t i = 0; i < transformNode->getChildren().size(); ++i)
    {
        print(transformNode->getChildren()[i]);
    }
}

void* GHBulletRagdollXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHModel* model = extraData.getProperty(GHEntityHashes::MODEL);
    if (!model) {
        GHDebugMessage::outputString("Error loading ragdoll: no model!");
        return 0;
    }
    
    GHTransformNode* transformNode = model->getSkeleton();
    GHBulletRagdoll* ret = new GHBulletRagdoll(mPhysicsSim, mControllerMgr, transformNode);
    populate(ret, node, extraData);
    return ret;
}

void GHBulletRagdollXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHBulletRagdoll* ret = reinterpret_cast<GHBulletRagdoll*>(obj);
    
    GHModel* model = extraData.getProperty(GHEntityHashes::MODEL);
    if (!model) {
        GHDebugMessage::outputString("Error loading ragdoll: no model!");
        return;
    }
    
    GHTransformNode* transformNode = model->getSkeleton();
    //print(transformNode);
    
    
    const char* mapFilename = node.getAttribute("map");
    GHXMLNode* separatelyLoadedMappingInfo = mXMLSerializer.loadXMLFile(mapFilename);
	const GHXMLNode* mappingInfo = separatelyLoadedMappingInfo;
    if (!mappingInfo) {
		mappingInfo = &node;
    }
    
    const GHXMLNode* bonesNodeParent = mappingInfo->getChild("bones", false);
    if (!bonesNodeParent) {
        GHDebugMessage::outputString("Error loading ragdoll: mapping file %s has no node named \"bones\". No ragdoll will be loaded.", mapFilename);
        return;
    }
    
    ret->setTransformNode(transformNode);
    
    const std::vector<GHXMLNode*>& boneNodes = bonesNodeParent->getChildren();
    size_t numBones = boneNodes.size();
    for (size_t i = 0; i < numBones; ++i)
    {
        const GHXMLNode* boneNode = boneNodes[i];
        createBone(*ret, transformNode, *boneNode, extraData);
    }
    
    const GHXMLNode* constraintsNodeParent = mappingInfo->getChild("constraints", false);
    if (constraintsNodeParent)
    {
        const std::vector<GHXMLNode*>& constraintsNodes = constraintsNodeParent->getChildren();
        size_t numConstraints = constraintsNodes.size();
        for (size_t i = 0; i < numConstraints; ++i)
        {
            const GHXMLNode* constraintsNode = constraintsNodes[i];
            createConstraint(*ret, *constraintsNode);
        }
    }
    
	delete separatelyLoadedMappingInfo;
    mBoneList.clear();
}

void GHBulletRagdollXMLLoader::createBone(GHBulletRagdoll& ret, GHTransformNode* skeleton, const GHXMLNode& xmlNode, GHPropertyContainer& extraData) const
{
    GHIdentifier nodeID = mIdFactory.generateHash(xmlNode.getName());
    GHTransformNode* renderBone = findTransformNodeByID(skeleton, nodeID);
    if (!renderBone)
    {
        GHDebugMessage::outputString("Error loading bone for ragdoll. Could not find node %s in skeleton.", xmlNode.getName().getChars());
        return;
    }

    GHBulletPhysicsObject* physicsBone = (GHBulletPhysicsObject*)mBulletObjectLoader.create(xmlNode, extraData);

    if (!physicsBone)
    {
        GHDebugMessage::outputString("There was an error loading the physics object for a ragdoll bone with id %s", xmlNode.getName().getChars());
        return;
    }
    
    physicsBone->setTransformNode(renderBone);

    //Add the bone/constraint to the ragdoll and store in our lookup map
    ret.addBone(physicsBone);
    mBoneList.push_back(physicsBone);
}

GHTransformNode* GHBulletRagdollXMLLoader::findTransformNodeByID(GHTransformNode* skeleton, const GHIdentifier& boneID) const
{
    if (skeleton->getId() == boneID) {
        return skeleton;
    }
    
    const std::vector<GHTransformNode*>& children = skeleton->getChildren();
    size_t numChildren = children.size();
    for (size_t i = 0; i < numChildren; ++i)
    {
        GHTransformNode* child = children[i];
        GHTransformNode* result = findTransformNodeByID(child, boneID);
        if (result) {
            return result;
        }
    }
    return 0;
}

GHBulletPhysicsObject* GHBulletRagdollXMLLoader::findRigidBodyByID(const GHIdentifier& boneID, int& outIndex) const
{
    size_t numBones = mBoneList.size();
    for (size_t i = 0; i < numBones; ++i)
    {
        GHBulletPhysicsObject* bone = mBoneList[i];
        if (bone->getTransformNode()->getId() == boneID) {
            outIndex = (int)i;
            return bone;
        }
    }
    outIndex = -1;
    return 0;
}

void GHBulletRagdollXMLLoader::createConstraint(GHBulletRagdoll& ret, const GHXMLNode& xmlNode) const
{
    GHIdentifier bodyAID = mIdFactory.generateHash(xmlNode.getAttribute("bodyA"));
    GHIdentifier bodyBID = mIdFactory.generateHash(xmlNode.getAttribute("bodyB"));
    
    int bodyAIndex = 0, bodyBIndex = 0;
    GHBulletPhysicsObject* bodyA = findRigidBodyByID(bodyAID, bodyAIndex);
    GHBulletPhysicsObject* bodyB = findRigidBodyByID(bodyBID, bodyBIndex);
    
    if (!bodyA || !bodyB)
    {
        GHDebugMessage::outputString("Error loading a constraint. Either or both of attributes bodyA or bodyB was not found in the list of bones. A constraint will not be constructed");
        return;
    }
    
    GHBulletRagdollConstraintXMLLoader constraintLoader;
    GHBulletRagdollConstraint* constraint = constraintLoader.loadConstraint(xmlNode, bodyA, bodyAIndex, bodyB, bodyBIndex);
    if (constraint) {
        ret.addConstraint(constraint);
    }
}
