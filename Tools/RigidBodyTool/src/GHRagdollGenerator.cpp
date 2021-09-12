// Copyright 2010 Golden Hammer Software
#include "GHRagdollGenerator.h"
#include "GHBulletRagdoll.h"
#include "GHModel.h"
#include "GHBulletPhysicsObject.h"
#include "btBulletDynamicsCommon.h"
#include "GHBulletUtil.h"
#include "GHBulletRagdollConstraint.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHModelPhysicsTransition.h"
#include "GHDebugMessage.h"
#include "GHRagdollToolNode.h"
#include "GHXMLNode.h"

GHRagdollGenerator::GHRagdollGenerator(const GHXMLObjFactory& objFactory,
                                       const GHIdentifierMap<int>& enumStore,
                                       GHEntity* targetEnt,
                                       GHPhysicsSim& physicsSim,
                                       GHControllerMgr& controllerMgr,
                                       const GHIdentifier& activeID,
                                       GHXMLNode* source)
: mXMLObjFactory(objFactory)
, mEnumStore(enumStore)
, mActiveID(activeID)
, mPhysicsSim(physicsSim)
, mControllerMgr(controllerMgr)
, mRagdoll(0)
, mTargetEnt(0)
, mTargetModel(0)
, mModelPhysicsTransition(0)
{
    setEntity(targetEnt);
    mTopXMLNode = source;
    
    if (!mTopXMLNode)
    {
        mTopXMLNode = new GHXMLNode;
        mTopXMLNode->setName("ragdoll", GHString::CHT_REFERENCE);
        GHXMLNode* bonesNode = new GHXMLNode;
        bonesNode->setName("bones", GHString::CHT_REFERENCE);
        mTopXMLNode->addChild(bonesNode);
        GHXMLNode* constraintsNode = new GHXMLNode;
        constraintsNode->setName("constraints", GHString::CHT_REFERENCE);
        mTopXMLNode->addChild(constraintsNode);
    }
    mRagdoll = new GHBulletRagdoll(reinterpret_cast<GHBulletPhysicsSim&>(mPhysicsSim), mControllerMgr, mTargetModel->getSkeleton());
}

GHRagdollGenerator::~GHRagdollGenerator(void)
{
    delete mTopXMLNode;
}

void GHRagdollGenerator::reset(GHEntity* targetEnt)
{
    setEntity(targetEnt);
    
    replaceSkeleton(mTargetModel->getSkeleton());
}

void GHRagdollGenerator::replaceSkeleton(GHTransformNode* skeleton)
{
    mRagdoll->setTopNode(skeleton);
    NodeMap::iterator iter = mPhysicsObjects.begin(), iterEnd = mPhysicsObjects.end();
    for (; iter != iterEnd; ++iter)
    {
        GHRagdollToolNode* toolNode = iter->second;
        toolNode->replaceSkeleton(skeleton);
    }
    attachRagdoll();
}

void GHRagdollGenerator::setEntity(GHEntity* targetEnt)
{
    detachRagdoll();
    mTargetEnt = targetEnt;
    mTargetModel = (GHModel*)targetEnt->mProperties.getProperty(GHEntityHashes::MODEL);
}

GHRagdollToolNode* GHRagdollGenerator::getNode(const GHIdentifier& id)
{
    NodeMap::iterator iter = mPhysicsObjects.find(id);
    if (iter == mPhysicsObjects.end()) {
        return 0;
    }
    return iter->second;
}

static void print(const GHTransformNode* transform)
{
    if (!transform) { return; }
    
    const char* parentStr = transform->getParent() ? transform->getParent()->getId().getString() : "none";
    GHDebugMessage::outputString("%s parent=%s", transform->getId().getString(), parentStr);
    
    size_t numChildren = transform->getChildren().size();
    for (size_t i = 0; i < numChildren; ++i)
    {
        print(transform->getChildren()[i]);
    }
}

void GHRagdollGenerator::generateRagdoll(void)
{
    print(mTargetModel->getSkeleton());
    mPhysicsObjects.clear();
    
    GHXMLNode* bonesNode = mTopXMLNode->getChild("bones", false);
    GHXMLNode* constraintsNode = mTopXMLNode->getChild("constraints", false);
    
    size_t numObjects = mDescription.mNodes.size();
    for (int i = 0; i < numObjects; ++i)
    {
        GHRagdollToolNode* toolNode = new GHRagdollToolNode(mXMLObjFactory,
                                                            mEnumStore,
                                                            mTargetModel->getSkeleton(),
                                                            *this,
                                                            mDescription.mNodes[i],
                                                            i);
        
        mRagdoll->addBone(toolNode->getPhysicsObject());
        mPhysicsObjects[mDescription.mNodes[i].mTargetID] = toolNode;
        
        bonesNode->addChild(toolNode->getRigidBodyNode());
    }
    
    NodeMap::iterator iter = mPhysicsObjects.begin(), iterEnd = mPhysicsObjects.end();
    for (; iter != iterEnd; ++iter)
    {
        GHRagdollToolNode* toolNode = iter->second;
        toolNode->generateConstraint();
        GHBulletRagdollConstraint* constraint = toolNode->getConstraint();
        if (constraint)
        {
            mRagdoll->addConstraint(constraint);
            constraintsNode->addChild(toolNode->getConstraintNode());
        }
    }
    
    attachRagdoll();
}

void GHRagdollGenerator::replaceRigidBody(int index, GHBulletPhysicsObject* newObj)
{
    mRagdoll->replaceBone(index, newObj);
    
    NodeMap::iterator iter = mPhysicsObjects.begin(), iterEnd = mPhysicsObjects.end();
    for (; iter != iterEnd; ++iter)
    {
        GHRagdollToolNode* toolNode = iter->second;
        if (!toolNode->getConstraint()) {
            continue;
        }
        
        int parentIndex, childIndex;
        toolNode->getConstraint()->getRigidBodyIndices(parentIndex, childIndex);
        if (parentIndex == index
            || childIndex == index)
        {
            mRagdoll->removeConstraint(toolNode->getConstraint());
            toolNode->loadConstraint();
            mRagdoll->addConstraint(toolNode->getConstraint());
        }
    }
}

void GHRagdollGenerator::setNode(const GHIdentifier& id, GHRagdollToolNode* node)
{
    mPhysicsObjects[id] = node;
    mRagdoll->addBone(node->getPhysicsObject());
}

#include "GHStringHashTableDJB2.h"
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"
#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"
#include "BulletDynamics/ConstraintSolver/btConeTwistConstraint.h"
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"

void GHRagdollGenerator::buildPregeneratedRagdoll(void)
{
    NodeMap::iterator iter = mPhysicsObjects.begin(), iterEnd = mPhysicsObjects.end();
    for (; iter != iterEnd; ++iter)
    {
        GHRagdollToolNode* toolNode = iter->second;
        if (toolNode->getConstraint()) {
            mRagdoll->addConstraint(toolNode->getConstraint());
        }
    }
    attachRagdoll();
    
    
    GHStringHashTableDJB2 hashtable;
    GHTransformNode* top = mTargetModel->getSkeleton();
    
    GHTransformNode* parent = top->findChild(hashtable.generateHash("spinelower"));
    GHTransformNode* us = top->findChild(hashtable.generateHash("spineupper"));
    GHTransformNode* child = top->findChild(hashtable.generateHash("lupperarm"));
    
    GHRagdollToolNode* parentToolNode = mPhysicsObjects.find(parent->getId())->second;
    GHRagdollToolNode* ourToolNode = mPhysicsObjects.find(us->getId())->second;
    GHRagdollToolNode* childToolNode = mPhysicsObjects.find(child->getId())->second;
    
    GHBulletPhysicsObject* parentPhysics = parentToolNode->getPhysicsObject();
    GHBulletPhysicsObject* ourPhysics = ourToolNode ? ourToolNode->getPhysicsObject() : 0;
    
    {
        GHTransform parentWorldInv;
        parentPhysics->getOffset().mult(parent->getTransform(), parentWorldInv);
        parentWorldInv.invert();
        
        //Our pivotInB is the our location in our local space, eg the identity
        
        GHTransform ourWorld;
        ourWorld = us->getTransform();
        
        GHTransform pivotInA;
        ourWorld.mult(parentWorldInv, pivotInA);
        
        pivotInA.debugPrint("pivotInA");
    }
    
    /*
    if (ourPhysics)
    {
        
        GHTransform oldPivotInA(0.962768, -0.219064, 0.158374, 0.000000, 0.218484, 0.975610, 0.021317, 0.000000, -0.159178, 0.014077, 0.987146, 0.000000, 0.036760, 0.197860, 0.014510, 1.000000);
        
        GHTransform oldParentWorld;
        parentPhysics->getOffset().mult(parent->getTransform(), oldParentWorld);
        
        GHTransform newParentWorld;
        ourPhysics->getOffset().mult(us->getTransform(), newParentWorld);
        GHTransform newParentWorldInv = newParentWorld;
        newParentWorldInv.invert();
        
        GHTransform pivotInAWorld;
        oldParentWorld.mult(oldPivotInA, pivotInAWorld);
        
        GHTransform finalPivotInA;
        pivotInAWorld.mult(newParentWorldInv, finalPivotInA);
        
        finalPivotInA.debugPrint("pivotInA");
    }
     */

}

void GHRagdollGenerator::attachRagdoll(void)
{
   // return;
    mTargetModel->setPhysicsObject(mRagdoll);
    mModelPhysicsTransition = new GHModelPhysicsTransition(*mTargetModel, mPhysicsSim);
    mTargetEnt->mStateMachine.addTransition(mActiveID, mModelPhysicsTransition);
}

void GHRagdollGenerator::detachRagdoll(void)
{
    //return;
    if (mModelPhysicsTransition)
    {
        mTargetEnt->mStateMachine.removeTransition(mModelPhysicsTransition);
        delete mModelPhysicsTransition;
        mModelPhysicsTransition = 0;
    }
    if (mTargetModel) {
        mTargetModel->setPhysicsObject(0);
    }
}

void GHRagdollGenerator::updatePivotTransforms(const GHIdentifier& nodeID, const GHTransform& oldOffset, const GHTransform& newOffset)
{
    NodeMap::iterator iter = mPhysicsObjects.begin(), iterEnd = mPhysicsObjects.end();
    for (; iter != iterEnd; ++iter)
    {
        GHRagdollToolNode* node = iter->second;
        GHXMLNode* constraintNode = node->getConstraintNode();
        if (constraintNode)
        {
            if (nodeID == mEnumStore.getHashTable().generateHash(constraintNode->getAttribute("bodyA")))
            {
                node->updatePivotTransform("pivotInATrans", oldOffset, newOffset);
            }
            else if (nodeID == mEnumStore.getHashTable().generateHash(constraintNode->getAttribute("bodyB")))
            {
                node->updatePivotTransform("pivotInBTrans", oldOffset, newOffset);
            }
        }
    }
}
