// Copyright Golden Hammer Software
#include "GHBulletRagdoll.h"
#include "GHTransformNode.h"
#include "GHBulletBtInclude.h"
#include "GHMath/GHPi.h"
#include "GHBulletPhysicsSim.h"
#include "GHBulletRagdollConstraint.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHBulletOffsetModifierTool.h"
#include "GHMath/GHFloat.h"

GHBulletRagdoll::GHBulletRagdoll(GHBulletPhysicsSim& physicsSim, GHControllerMgr& controllerMgr, GHTransformNode* skeleton)
: mPhysicsSim(physicsSim)
, mControllerMgr(controllerMgr)
, mTopNodeUpdater(skeleton, mBones)
, mOffsetModifier(0)
, mIsActive(false)
{
    
}

GHBulletRagdoll::~GHBulletRagdoll(void)
{
    size_t numConstraints = mConstraints.size();
    for (size_t i = 0; i < numConstraints; ++i) {
        delete mConstraints[i];
    }
    
    size_t numBones = mBones.size();
    for (size_t i = 0; i < numBones; ++i) {
        delete mBones[i];
    }
    
    delete mOffsetModifier;
}

void GHBulletRagdoll::setTopNode(GHTransformNode* skeleton)
{
    mTopNodeUpdater.setTopNode(skeleton);
}

void GHBulletRagdoll::activate(void)
{
    size_t numBones = mBones.size();
    for (size_t i = 0; i < numBones; ++i)
    {
        mBones[i]->activate();
    }
    
    size_t numConstraints = mConstraints.size();
    for (size_t i = 0; i < numConstraints; ++i)
    {
        mPhysicsSim.addToSim(mConstraints[i]->getBulletConstraint(), mConstraints[i]->bodyCollisionDisabled());
    }
    
    mControllerMgr.add(&mTopNodeUpdater);
    
    if (mOffsetModifier)
    {
        mControllerMgr.add(mOffsetModifier);
    }
    mIsActive = true;
}

void GHBulletRagdoll::deactivate(void)
{
    if (mOffsetModifier)
    {
        mControllerMgr.remove(mOffsetModifier);
    }
    
    mControllerMgr.remove(&mTopNodeUpdater);
    int numConstraints = (int)mConstraints.size();
    for (int i = numConstraints - 1; i >= 0; --i)
    {
        mPhysicsSim.removeFromSim(mConstraints[i]->getBulletConstraint());
    }
    
    int numBones = (int)mBones.size();
    for (int i = numBones - 1; i >= 0; --i)
    {
        mBones[i]->deactivate();
    }
    mIsActive = false;
}

void GHBulletRagdoll::addBone(GHBulletPhysicsObject* bone)
{
    assert(bone);
    mBones.push_back(bone);
}

void GHBulletRagdoll::replaceBone(int index, GHBulletPhysicsObject* bone)
{
    if (mIsActive)
    {
        mBones[index]->deactivate();
        bone->activate();
    }
    mBones[index] = bone;
}

void GHBulletRagdoll::removeConstraint(GHBulletRagdollConstraint* constraint)
{
    std::vector<GHBulletRagdollConstraint*>::iterator iter = std::find(mConstraints.begin(), mConstraints.end(), constraint);
    if (iter != mConstraints.end())
    {
        if (mIsActive)
        {
            mPhysicsSim.removeFromSim(constraint->getBulletConstraint());
        }
        mConstraints.erase(iter);
    }
}

void GHBulletRagdoll::addConstraint(GHBulletRagdollConstraint* constraint)
{
    mConstraints.push_back(constraint);
    if (mIsActive)
    {
        mPhysicsSim.addToSim(constraint->getBulletConstraint(), constraint->bodyCollisionDisabled());
    }
}

GHPhysicsObject* GHBulletRagdoll::clone(GHTransformNode* skeleton)
{
    GHBulletRagdoll* myClone = new GHBulletRagdoll(mPhysicsSim, mControllerMgr, skeleton);
    
    std::vector<GHTransformNode*> oldNodes, newNodes;
    getTransformNode()->collectTree(oldNodes);
    skeleton->collectTree(newNodes);
    
    size_t numBones = mBones.size();
    for (size_t boneIdx = 0; boneIdx < numBones; ++boneIdx)
    {
        GHTransformNode* newNode = 0;
        for (size_t nodeIdx = 0; nodeIdx < oldNodes.size(); ++nodeIdx)
        {
            if (mBones[boneIdx]->getTransformNode() == oldNodes[nodeIdx])
            {
                newNode = newNodes[nodeIdx];
                break;
            }
        }
        myClone->addBone(mBones[boneIdx]->clone(newNode));
    }
    
    size_t numConstraints = mConstraints.size();
    for (size_t i = 0; i < numConstraints; ++i)
    {
        GHBulletRagdollConstraint* constraint = mConstraints[i];
        
        int parentIndex, childIndex;
        constraint->getRigidBodyIndices(parentIndex, childIndex);
        
        GHBulletPhysicsObject* parent = myClone->mBones[parentIndex];
        GHBulletPhysicsObject* child = myClone->mBones[childIndex];
        
        GHBulletRagdollConstraint* newConstraint = constraint->clone(parent->getBulletObject(), child->getBulletObject());
        myClone->addConstraint(newConstraint);
    }
    
    if (mOffsetModifier)
    {
        myClone->setOffsetModifier(mOffsetModifier->clone(myClone->mBones));
    }
    
    return myClone;
}

void GHBulletRagdoll::getLinearVelocity(GHPoint3& linearVelocity) const
{
    linearVelocity.setCoords(0,0,0);
    size_t numBodies = mBones.size();
    for (size_t i = 0; i < numBodies; ++i)
    {
        GHBulletPhysicsObject* obj = mBones[i];
        GHPoint3 objVel;
        obj->getLinearVelocity(objVel);
        linearVelocity += objVel;
    }
    linearVelocity /= (float)numBodies;
}

void GHBulletRagdoll::getAngularVelocity(GHPoint3& angularVelocity) const
{
    angularVelocity.setCoords(0,0,0);
}

void GHBulletRagdoll::setLinearVelocity(const GHPoint3& linearVelocity)
{
    size_t numBodies = mBones.size();
    for (size_t i = 0; i < numBodies; ++i)
    {
        GHBulletPhysicsObject* obj = mBones[i];
        obj->setLinearVelocity(linearVelocity);
    }
}

void GHBulletRagdoll::setAngularVelocity(const GHPoint3& angularVelocity)
{
    size_t numBodies = mBones.size();
    for (size_t i = 0; i < numBodies; ++i)
    {
        GHBulletPhysicsObject* obj = mBones[i];
        obj->setAngularVelocity(angularVelocity);
    }
}

void GHBulletRagdoll::setUserData(void* userData)
{
	size_t numBodies = mBones.size();
	for (size_t i = 0; i < numBodies; ++i)
	{
		GHBulletPhysicsObject* obj = mBones[i];
		obj->setUserData(userData);
	}
}

float GHBulletRagdoll::getMass(void) const
{
    return 0;
}

void GHBulletRagdoll::setFriction(float friction)
{
    size_t numBodies = mBones.size();
    for (size_t i = 0; i < numBodies; ++i)
    {
        GHBulletPhysicsObject* obj = mBones[i];
        obj->setFriction(friction);
    }
}

float GHBulletRagdoll::getFriction(void) const
{
    return mBones[0]->getFriction();
}

void GHBulletRagdoll::setRestitution(float restitution)
{
    size_t numBodies = mBones.size();
    for (size_t i = 0; i < numBodies; ++i)
    {
        GHBulletPhysicsObject* obj = mBones[i];
        obj->setRestitution(restitution);
    }
}

float GHBulletRagdoll::getRestitution(void) const
{
    return mBones[0]->getRestitution();
}

void GHBulletRagdoll::getCenterOfMass(GHPoint3& outCoM) const
{
    size_t numBodies = mBones.size();
    float totalMass = 0;
    outCoM.setCoords(0, 0, 0);
    for (size_t i = 0; i < numBodies; ++i)
    {
        GHBulletPhysicsObject* obj = mBones[i];

        GHPoint3 com;
        obj->getCenterOfMass(com);
        com *= obj->getMass();
        totalMass += obj->getMass();
    }

    if (!GHFloat::isZero(totalMass)) {
        outCoM /= totalMass;
    }
}

void GHBulletRagdoll::setAngularFactor(const GHPoint3& angularFactor)
{
	//todo?? What does this mean for a ragdoll?
	assert(false);
}

void GHBulletRagdoll::setCollisionLayer(short collisionLayer)
{
    assert(false && "GHBulletRagdoll::setCollisionLayer unimplemented and untested (note - GHBulletPhysicsObject adds/removes from sim. this might cause issues)");
}

void GHBulletRagdoll::applyForce(const GHPoint3& force, const GHPoint3& position)
{
    mBones[0]->applyForce(force, position);
}

void GHBulletRagdoll::applyImpulse(const GHPoint3& impulse, const GHPoint3& position)
{
    mBones[0]->applyImpulse(impulse, position);
}

void GHBulletRagdoll::applyTorque(const GHPoint3& torque)
{
    mBones[0]->applyTorque(torque);
}

void GHBulletRagdoll::applyAngularImpulse(const GHPoint3& impulse)
{
    mBones[0]->applyAngularImpulse(impulse);
}

void GHBulletRagdoll::resetMotion(void)
{
    size_t numBones = mBones.size();
    for (size_t i = 0; i < numBones; ++i)
    {
        mBones[i]->resetMotion();
    }
}

GHBulletRagdoll::TopNodeUpdater::TopNodeUpdater(GHTransformNode* topNode, std::vector<GHBulletPhysicsObject*>& boneList)
: mTopNode(topNode)
, mBoneList(boneList)
{
    
}

void GHBulletRagdoll::TopNodeUpdater::setTopNode(GHTransformNode* topNode)
{
    mTopNode = topNode;
}

void GHBulletRagdoll::TopNodeUpdater::onActivate(void)
{
    if (!mBoneList.size()) {
        mRelativeTransform.becomeIdentity();
        return;
    }
    
    mRelativeTransform = mBoneList[0]->getTransformNode()->getTransform();
    mRelativeTransform.invert();
    mTopNode->getTransform().mult(mRelativeTransform, mRelativeTransform);
}

void GHBulletRagdoll::TopNodeUpdater::onDeactivate(void)
{
    
}

void GHBulletRagdoll::TopNodeUpdater::update(void)
{
    if (!mBoneList.size()) {
        return;
    }
    
    mTopNode->getLocalTransform() = mBoneList[0]->getTransform();
    mRelativeTransform.mult(mTopNode->getLocalTransform(), mTopNode->getLocalTransform());

    size_t numBones = mBoneList.size();
    for (size_t i = 0; i < numBones; ++i)
    {
        GHPhysicsObject* bone = mBoneList[i];
        bone->negateManualPosition();
    }
}
