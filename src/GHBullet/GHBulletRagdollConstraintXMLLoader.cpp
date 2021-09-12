// Copyright Golden Hammer Software
#include "GHBulletRagdollConstraintXMLLoader.h"
#include "GHBulletRagdollConstraint.h"
#include "GHBulletBtInclude.h"
#include "GHBulletPhysicsObject.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include <string.h>
#include "GHBulletUtil.h"

GHBulletRagdollConstraint* GHBulletRagdollConstraintXMLLoader::loadConstraint(const GHXMLNode& xmlNode,
                                                                              GHBulletPhysicsObject* parent, int parentIndex,
                                                                              GHBulletPhysicsObject* child, int childIndex) const
{
    btTypedConstraint* bulletConstraint = loadBulletConstraint(xmlNode, parent->getBulletObject(), child->getBulletObject());
    if (!bulletConstraint) {
        return 0;
    }
    
    bool bodyCollisionDisabled = false;
    xmlNode.readAttrBool("disableCollision", bodyCollisionDisabled);
    
    GHBulletRagdollConstraint* ret = new GHBulletRagdollConstraint(bulletConstraint, parentIndex, childIndex, bodyCollisionDisabled);
    return ret;
}

btTypedConstraint* GHBulletRagdollConstraintXMLLoader::loadBulletConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const
{
    //Important: If you add a new type, make sure to update GHBulletRagdollConstraint::clone
    
    const char* typeStr = xmlNode.getName().getChars();
    
    if (!strcmp(typeStr, "hinge")) {
        return loadHingeConstraint(xmlNode, parent, child);
    }
    else if(!strcmp(typeStr, "point2point")) {
        return loadPoint2PointConstraint(xmlNode, parent, child);
    }
    else if(!strcmp(typeStr, "coneTwist")) {
        return loadConeTwistConstraint(xmlNode, parent, child);
    }
    else if (!strcmp(typeStr, "slider")) {
        return loadSliderConstraint(xmlNode, parent, child);
    }
    else if (!strcmp(typeStr, "6DOF")) {
        return load6DOFConstraint(xmlNode, parent, child);
    }
    //todo: add more options
    else {
        GHDebugMessage::outputString("Error loading constraint: the type \"%s\" is not recognized. The constraint will not be loaded.", typeStr);
        return 0;
    }
}

btTypedConstraint* GHBulletRagdollConstraintXMLLoader::loadHingeConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const
{
    //Important: If you change this method, make sure to update GHBulletRagdollConstraint::cloneHinge
    btTransform frameA;
    readFrame(xmlNode, "pivotInATrans", "pivotInA", frameA);
    
    btTransform frameB;
    readFrame(xmlNode, "pivotInBTrans", "pivotInB", frameB);
    
    float floatArr[3];
    btVector3 axisInA(1, 0, 0);
    btVector3 axisInB(1, 0, 0);
    
    if (xmlNode.readAttrFloatArr("axis", floatArr, 3)) {
        axisInA = btVector3(floatArr[0], floatArr[1], floatArr[2]);
        axisInB = btVector3(floatArr[0], floatArr[1], floatArr[2]);
    }
    else {
        if (xmlNode.readAttrFloatArr("axisInA", floatArr, 3)) {
            axisInA = btVector3(floatArr[0], floatArr[1], floatArr[2]);
        }
        
        if (xmlNode.readAttrFloatArr("axisInB", floatArr, 3)) {
            axisInB = btVector3(floatArr[0], floatArr[1], floatArr[2]);
        }
    }
    
    btHingeConstraint* ret = new btHingeConstraint(*parent, *child, frameA.getOrigin(), frameB.getOrigin(), axisInA, axisInB, true);

    float limit[2];
    if (xmlNode.readAttrFloatArr("limit", limit, 2)) {
        ret->setLimit(limit[0]*SIMD_PI, limit[1]*SIMD_PI);
    }
    
    return ret;
}

btTypedConstraint* GHBulletRagdollConstraintXMLLoader::loadPoint2PointConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const
{
    //Important: If you change this method, make sure to update GHBulletRagdollConstraint::clonePoint2Point
    
    float floatArr[3];
    
    xmlNode.readAttrFloatArr("pivotInA", floatArr, 3);
    btVector3 pivotInA(floatArr[0], floatArr[1], floatArr[2]);
    
    xmlNode.readAttrFloatArr("pivotInB", floatArr, 3);
    btVector3 pivotInB(floatArr[0], floatArr[1], floatArr[2]);
    
    return new btPoint2PointConstraint(*parent, *child, pivotInA, pivotInB);
}

void GHBulletRagdollConstraintXMLLoader::readFrame(const GHXMLNode& xmlNode, const char* transAttr, const char* posAttr, btTransform& outTrans) const
{
    //read pivotInATrans (or equivalent) first. if it exists, it takes precedent
    GHTransform ghTrans;
    if (xmlNode.readAttrFloatArr(transAttr, ghTrans.getMatrix().getArr(), 16))
    {
        GHBulletUtil::ghToBulletTransform(ghTrans, outTrans);
        return;
    }
    //if pivotInATrans (or equivalent) doesn't exist, then we look for
    // pivotInA (or equivalent)
    else
    {
		float floatArr[3] = { 0 };
        xmlNode.readAttrFloatArr(posAttr, floatArr, 3);
        
        outTrans.setIdentity();
        outTrans.setOrigin(btVector3(floatArr[0], floatArr[1], floatArr[2]));
    }
}

void GHBulletRagdollConstraintXMLLoader::readBTVector(const GHXMLNode& xmlNode, const char* attr, btVector3& outVector) const
{
    float floatArr[3];
    xmlNode.readAttrFloatArr(attr, floatArr, 3);
    outVector[0] = floatArr[0];
    outVector[1] = floatArr[1];
    outVector[2] = floatArr[2];
}

btTypedConstraint* GHBulletRagdollConstraintXMLLoader::loadConeTwistConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const
{
    //Important: If you change this method, make sure to update GHBulletRagdollConstraint::cloneConeTwist
    
    float floatArr[3];
    
    btTransform frameA;
    readFrame(xmlNode, "pivotInATrans", "pivotInA", frameA);
    
    btTransform frameB;
    readFrame(xmlNode, "pivotInBTrans", "pivotInB", frameB);
    
    btConeTwistConstraint* ret = new btConeTwistConstraint(*parent, *child, frameA, frameB);

    if (xmlNode.readAttrFloatArr("limit", floatArr, 3)) {
        ret->setLimit(floatArr[0]*SIMD_PI, floatArr[1]*SIMD_PI, floatArr[2]*SIMD_PI);
    }
    
    return ret;
}

btTypedConstraint*  GHBulletRagdollConstraintXMLLoader::load6DOFConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const
{
    //Important: If you change this method, make sure to update GHBulletRagdollConstraint::clone6DOF
    btTransform frameA;
    readFrame(xmlNode, "pivotInATrans", "pivotInA", frameA);
    
    btTransform frameB;
    readFrame(xmlNode, "pivotInBTrans", "pivotInB", frameB);
    
    bool useLinearReferenceFrameA = true;
    
    btGeneric6DofConstraint* ret = new btGeneric6DofConstraint(*parent, *child, frameA, frameB, useLinearReferenceFrameA);

    btVector3 angularLower(0,0,0);
    readBTVector(xmlNode, "angularLower", angularLower);
    btVector3 angularUpper = angularLower;
    readBTVector(xmlNode, "angularUpper", angularUpper);
    btVector3 linearLower(0,0,0);
    readBTVector(xmlNode, "linearLower", linearLower);
    btVector3 linearUpper = linearLower;
    readBTVector(xmlNode, "linearUpper", linearUpper);
    
    ret->setAngularLowerLimit(angularLower);
    ret->setAngularUpperLimit(angularUpper);
    ret->setLinearLowerLimit(linearLower);
    ret->setLinearUpperLimit(linearUpper);
    
    return ret;
}

btTypedConstraint* GHBulletRagdollConstraintXMLLoader::loadSliderConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const
{
    //Important: If you change this method, make sure to update GHBulletRagdollConstraint::cloneSlider
    
    btTransform frameA;
    readFrame(xmlNode, "pivotInATrans", "pivotInA", frameA);
    
    btTransform frameB;
    readFrame(xmlNode, "pivotInBTrans", "pivotInB", frameB);
    
    bool useLinearReferenceFrameA = true;
    btSliderConstraint* ret = new btSliderConstraint(*parent, *child, frameA, frameB, useLinearReferenceFrameA);
    
    return ret;
}

