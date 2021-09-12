// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolNode.h"
#include "GHXMLNode.h"
#include "GHTransformNode.h"
#include <cstring>
#include "GHXMLObjFactory.h"
#include "GHPropertyContainer.h"
#include "GHBulletPhysicsObject.h"
#include "GHRagdollGenerator.h"
#include "GHBulletRagdollConstraintXMLLoader.h"
#include "GHBulletRagdollConstraint.h"
#include "GHDebugMessage.h"

namespace  {
    void sprintfTransform(char* buffer, const int bufLen, const GHTransform& transform)
    {
        snprintf(buffer, bufLen, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                 transform.getMatrix()[0], transform.getMatrix()[1], transform.getMatrix()[2], transform.getMatrix()[3],
                 transform.getMatrix()[4], transform.getMatrix()[5], transform.getMatrix()[6], transform.getMatrix()[7],
                 transform.getMatrix()[8], transform.getMatrix()[9], transform.getMatrix()[10], transform.getMatrix()[11],
                 transform.getMatrix()[12], transform.getMatrix()[13], transform.getMatrix()[14], transform.getMatrix()[15]);
    }
}

GHRagdollToolNode::GHRagdollToolNode(const GHXMLObjFactory& objFactory,
                                     const GHIdentifierMap<int>& enumStore,
                                     GHTransformNode* parentSkeleton,
                                     GHRagdollGenerator& parentGenerator,
                                     GHRagdollGeneratorDescription::NodeDesc& nodeDesc,
                                     int index)
: mXMLObjFactory(objFactory)
, mEnumStore(enumStore)
, mParentSkeleton(parentSkeleton)
, mParentGenerator(parentGenerator)
, mDesc(nodeDesc)
, mIndex(index)
, mPhysicsObject(0)
, mConstraint(0)
, mConstraintNode(0)
, mRigidBodyNode(0)
{
    createRigidBody();
}

GHRagdollToolNode::GHRagdollToolNode(const GHXMLObjFactory& objFactory,
                                     const GHIdentifierMap<int>& enumStore,
                                     GHTransformNode* parentSkeleton,
                                     GHRagdollGenerator& parentGenerator,
                                     GHXMLNode* rigidBodyNode,
                                     int index)
: mXMLObjFactory(objFactory)
, mEnumStore(enumStore)
, mParentSkeleton(parentSkeleton)
, mParentGenerator(parentGenerator)
, mRigidBodyNode(rigidBodyNode)
, mIndex(index)
, mPhysicsObject(0)
, mConstraint(0)
, mConstraintNode(0)
{
    mDesc.mTargetName = mRigidBodyNode->getName();
    mDesc.mTargetID = mEnumStore.getHashTable().generateHash(mRigidBodyNode->getName());
    loadRigidBodyFromXML();
}

void GHRagdollToolNode::replaceSkeleton(GHTransformNode* skeleton)
{
    mParentSkeleton = skeleton;
    
    mPhysicsObject->setTransformNode(skeleton->findChild(mDesc.mTargetID));
}

void GHRagdollToolNode::setConstraintNode(GHXMLNode* constraintNode)
{
    mConstraintNode = constraintNode;
    
    mDesc.mParentName = mConstraintNode->getAttribute("bodyA");
    mDesc.mParentID = mEnumStore.getHashTable().generateHash(mDesc.mParentName);
    
    loadConstraint();
}

void GHRagdollToolNode::loadRigidBodyFromXML(void)
{
    mRigidBodyNode->setName("physics", GHString::CHT_REFERENCE);
    mPhysicsObject = (GHBulletPhysicsObject*)mXMLObjFactory.load(*mRigidBodyNode);
    mRigidBodyNode->setName(mDesc.mTargetName, GHString::CHT_COPY);
    GHTransformNode* ourNode = mParentSkeleton->findChild(mDesc.mTargetID);
    mPhysicsObject->setTransformNode(ourNode);
}

void GHRagdollToolNode::createRigidBody(void)
{
    generateRigidBodyXML();
    loadRigidBodyFromXML();
}

void GHRagdollToolNode::reloadRigidBody(void)
{
    GHBulletPhysicsObject* oldObj = mPhysicsObject;
    loadRigidBodyFromXML();
    mPhysicsObject->setTransformNode(oldObj->getTransformNode());
    
    mParentGenerator.replaceRigidBody(mIndex, mPhysicsObject);
    
    delete oldObj;
}

GHXMLNode* GHRagdollToolNode::getShapeNode(void)
{
    GHXMLNode* shapeNode = mRigidBodyNode->getChild("shape", false);
    if (!shapeNode) {
        shapeNode = new GHXMLNode;
        mRigidBodyNode->addChild(shapeNode);
    }
    return shapeNode;
}

bool GHRagdollToolNode::setShape(const char* shapeStr)
{
    //at least make sure the string is in the map at all. No guarantee
    // that it's a valid shape string, but this should protect against
    // casual typos
    if(!mEnumStore.find(shapeStr)) {
        return false;
    }
    
    GHXMLNode* shapeNode = getShapeNode();
    
    shapeNode->setAttribute("type", GHString::CHT_REFERENCE,
                            shapeStr, GHString::CHT_COPY);
    
    reloadRigidBody();
    
    return true;
}

bool GHRagdollToolNode::setLimit(const char* limit)
{
    GHXMLNode* constraintNode = getConstraintNode();
    if (!constraintNode) { return false; }
    
    constraintNode->setAttribute("limit", GHString::CHT_REFERENCE,
                                 limit, GHString::CHT_COPY);
    
    mParentGenerator.replaceRigidBody(mIndex, mPhysicsObject);
    return true;
}

bool GHRagdollToolNode::setConstraintType(const char* constraintType)
{
    GHXMLNode* constraintNode = getConstraintNode();
    if (!constraintNode) { return false; }
    
    constraintNode->setName(constraintType, GHString::CHT_COPY);
    
    mParentGenerator.replaceRigidBody(mIndex, mPhysicsObject);
    return true;
}

bool GHRagdollToolNode::setAxis(const char* axis)
{
    GHXMLNode* constraintNode = getConstraintNode();
    if (!constraintNode) { return false; }
    
    constraintNode->setAttribute("axis", GHString::CHT_REFERENCE,
                                 axis, GHString::CHT_COPY);
    
    mParentGenerator.replaceRigidBody(mIndex, mPhysicsObject);
    return true;
}

bool GHRagdollToolNode::setExtent(int index, float value)
{
    GHXMLNode* shapeNode = getShapeNode();
    GHPoint3 extents(1,1,1);
    shapeNode->readAttrFloatArr("extents", extents.getArr(), 3);
    extents[index] = value;
    const int bufLen = 256;
    char buffer[bufLen];
    snprintf(buffer, bufLen, "%f %f %f", extents[0], extents[1], extents[2]);
    shapeNode->setAttribute("extents", GHString::CHT_REFERENCE,
                            buffer, GHString::CHT_COPY);
    
    reloadRigidBody();
    return true;
}

bool GHRagdollToolNode::setRadius(float radius)
{
    GHXMLNode* shapeNode = getShapeNode();
    const int bufLen = 64;
    char buffer[bufLen];
    snprintf(buffer, bufLen, "%f", radius);
    shapeNode->setAttribute("radius", GHString::CHT_REFERENCE,
                            buffer, GHString::CHT_COPY);
    
    reloadRigidBody();
    
    return true;
}

void GHRagdollToolNode::applyOffsetChange(const GHTransform& adjustment)
{
    GHTransform oldOffset = mPhysicsObject->getOffset();
    
    GHTransform newOffset;
    adjustment.mult(oldOffset, newOffset);
    
    GHTransform oldTransform, newTransformInv;
    GHTransform nodeTrans = mPhysicsObject->getTransformNode()->getTransform();
    oldOffset.mult(nodeTrans, oldTransform);
    newOffset.mult(nodeTrans, newTransformInv);
    newTransformInv.invert();
    
    mParentGenerator.updatePivotTransforms(mPhysicsObject->getTransformNode()->getId(), oldTransform, newTransformInv);
    
    mPhysicsObject->setOffset(newOffset);
    
    const int bufLen=256;
    char buffer[bufLen];
    sprintfTransform(buffer, bufLen, newOffset);
    mRigidBodyNode->setAttribute("offsetTransform", GHString::CHT_REFERENCE,
                                 buffer, GHString::CHT_COPY);
    
    mParentGenerator.replaceRigidBody(mIndex, mPhysicsObject);
    //mParentGenerator.handleOffsetChanged(mIndex, oldOffset, newOffset);
}

void GHRagdollToolNode::generateRigidBodyXML(void)
{
    GHXMLNode* ret = new GHXMLNode;
    
    ret->setName(mDesc.mTargetName, GHString::CHT_REFERENCE);
    
    ret->setAttribute("dynamic", GHString::CHT_REFERENCE, "true", GHString::CHT_REFERENCE);
    
    GHTransform offset;
    GHXMLNode* shapeNode = generateShapeXML(offset);
    ret->addChild(shapeNode);
    
    const int BUF_SZ = 256;
    char buf[BUF_SZ];
    sprintfTransform(buf, BUF_SZ, offset);
    
    ret->setAttribute("offsetTransform", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
    
    mRigidBodyNode = ret;
}

GHXMLNode* GHRagdollToolNode::generateShapeXML(GHTransform& outOffset)
{
    bool isSphere = false;
    //If the target and pivot are the same, then we are a sphere.
    if (mDesc.mTargetID == mDesc.mPivotID)
    {
        outOffset.becomeIdentity();
        isSphere = true;
    }
    
    //We are also a sphere if a center was specified, but we have to calculate an offset
    if (!mDesc.mCenterID.isNull())
    {
        GHTransformNode* target = mParentSkeleton->findChild(mDesc.mTargetID);
        GHTransformNode* center = mParentSkeleton->findChild(mDesc.mCenterID);
        if (target && center)
        {
            GHTransform centerInv = center->getTransform();
            centerInv.invert();
            
            target->getTransform().mult(centerInv, outOffset);
        }
        else
        {
            outOffset.becomeIdentity();
        }
        isSphere = true;
    }
    
    if (isSphere)
    {
        GHXMLNode* ret = new GHXMLNode;
        ret->setName("shape", GHString::CHT_REFERENCE);
        ret->setAttribute("type", GHString::CHT_REFERENCE, "PS_SPHERE", GHString::CHT_REFERENCE);
        const int BUF_SZ = 50;
        char buf[BUF_SZ];
        snprintf(buf, BUF_SZ, "%f", mDesc.mRadius);
        ret->setAttribute("radius", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
        return ret;
    }
    
    GHTransformNode* target = mParentSkeleton->findChild(mDesc.mTargetID);
    GHTransformNode* pivot = mParentSkeleton->findChild(mDesc.mPivotID);
    
    //Calculate three values:
    // diffVec = the normalized vector pointing from the target to the pivot
    // centerPos = the world position lying equidistant between the target and the pivot
    // dist = the distance between the target and pivot points
    GHPoint3 targetPos, pivotPos;
    target->getTransform().getTranslate(targetPos);
    pivot->getTransform().getTranslate(pivotPos);
    GHPoint3 diffVec = pivotPos;
    diffVec -= targetPos;
    GHPoint3 centerPos = diffVec;
    centerPos *= .5f;
    centerPos += targetPos;
    float dist = diffVec.normalize();
    
    //Convert diffVec and centerPos to the target's local space
    GHTransform targetInv = target->getTransform();
    targetInv.invert();
    targetInv.multDir(diffVec, diffVec);
    targetInv.mult(centerPos, centerPos);
    
    //Use dist and radius to figure out the length of the capsule
    float buffer = .0125f;
    float height = (dist - (mDesc.mRadius+buffer)*2.0);
    
    //Calculate the offset from the target to the rigid body using diffVec.
    // For simplicity, all capsules are down the z axis
    outOffset.becomeRotationBetweenVectors(GHPoint3(0,0,1), diffVec);
    outOffset.setTranslate(centerPos);
    
    //Generate the shape XML using the above data
    GHXMLNode* ret = new GHXMLNode;
    ret->setName("shape", GHString::CHT_REFERENCE);
    ret->setAttribute("type", GHString::CHT_REFERENCE, "PS_CAPSULE", GHString::CHT_REFERENCE);
    //Again, for simplicity all capsules are down the z axis
    ret->setAttribute("axis", GHString::CHT_REFERENCE, "2", GHString::CHT_REFERENCE);
    const int BUF_SZ = 50;
    char buf[BUF_SZ];
    snprintf(buf, BUF_SZ, "%f", mDesc.mRadius);
    ret->setAttribute("radius", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
    snprintf(buf, BUF_SZ, "%f", height);
    ret->setAttribute("height", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
    
    return ret;
}

void GHRagdollToolNode::generateConstraint(void)
{
    if (mDesc.mParentID == mDesc.mTargetID) {
        return;
    }
    
    delete mConstraint;
    mConstraint = 0;
    
    delete mConstraintNode;
    
    GHRagdollToolNode* ghParent = mParentGenerator.getNode(mDesc.mParentID);
    GHRagdollToolNode* ghChild = this;
    
    GHTransformNode* pivotNode = mParentSkeleton->findChild(mDesc.mPivotID);
    
    //Calculate the transform that brings us from world space into the
    // parent's local space;
    GHTransform worldToParentLocal;
    ghParent->getPhysicsObject()->getOffset().mult(ghParent->getPhysicsObject()->getTransformNode()->getTransform(), worldToParentLocal);
    //ghParent->getPhysicsObject()->getTransformNode()->getTransform().mult(ghParent->getPhysicsObject()->getOffset(), worldToParentLocal);
    worldToParentLocal.invert();
    
    //Multiply this by the pivot point to get the pivotInA
    GHTransform pivotInATrans;
    pivotNode->getTransform().mult(worldToParentLocal, pivotInATrans);
    //worldToParentLocal.mult(pivotNode->getTransform(), pivotInATrans);
    
    //Now calculate the transform that brings us from world space into the
    // child's local space
    GHTransform worldToChildLocal;
    ghChild->getPhysicsObject()->getOffset().mult(ghChild->getPhysicsObject()->getTransformNode()->getTransform(), worldToChildLocal);
    //ghChild->getPhysicsObject()->getTransformNode()->getTransform().mult(ghChild->getPhysicsObject()->getOffset(), worldToChildLocal);
    worldToChildLocal.invert();
    
    //Multiply this by the pivot point to get the pivotInB
    GHTransform pivotInBTrans;
    pivotNode->getTransform().mult(worldToChildLocal, pivotInBTrans);
    //worldToChildLocal.mult(pivotNode->getTransform(), pivotInBTrans);
    
    GHXMLNode* ret = new GHXMLNode;
    ret->setName("coneTwist", GHString::CHT_REFERENCE);
    
    ret->setAttribute("bodyA", GHString::CHT_REFERENCE, mDesc.mParentName, GHString::CHT_COPY);
    ret->setAttribute("bodyB", GHString::CHT_REFERENCE, mDesc.mTargetName, GHString::CHT_COPY);
    
    const int BUF_SZ = 256;
    char buf[BUF_SZ];
    sprintfTransform(buf, BUF_SZ, pivotInATrans);
    ret->setAttribute("pivotInATrans", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
    
    sprintfTransform(buf, BUF_SZ, pivotInBTrans);
    ret->setAttribute("pivotInBTrans", GHString::CHT_REFERENCE, buf, GHString::CHT_COPY);
    
    ret->setAttribute("limit", GHString::CHT_REFERENCE, "0 0 0", GHString::CHT_REFERENCE);
    
    mConstraintNode = ret;

    loadConstraint();
}

void GHRagdollToolNode::loadConstraint(void)
{
    if (mConstraint) {
        delete mConstraint;
    }
    GHRagdollToolNode* ghParent = mParentGenerator.getNode(mDesc.mParentID);
    GHRagdollToolNode* ghChild = this;
    GHBulletRagdollConstraintXMLLoader constraintLoader;
    mConstraint = constraintLoader.loadConstraint(*mConstraintNode, ghParent->getPhysicsObject(), ghParent->getIndex(), ghChild->getPhysicsObject(), ghChild->getIndex());
}

void GHRagdollToolNode::updatePivotTransform(const char* pivotAttr, const GHTransform& oldTransform, const GHTransform& newTransformInv)
{
    if (!mConstraintNode) { return; }
    GHTransform pivotTrans;
    mConstraintNode->readAttrFloatArr(pivotAttr, pivotTrans.getMatrix().getArr(), 16);
    
    //pivotTrans is wrt oldTransform.
    // Mult by oldTransform to get in worldspace
    // then mult by newTransformInv to become wrt to newTransform
    
    pivotTrans.mult(oldTransform, pivotTrans);
    pivotTrans.mult(newTransformInv, pivotTrans);
    
    const int buflen = 256;
    char buffer[buflen];
    sprintfTransform(buffer, buflen, pivotTrans);
    mConstraintNode->setAttribute(pivotAttr, GHString::CHT_REFERENCE,
                                  buffer, GHString::CHT_COPY);
}
