// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRagdollGeneratorDescription.h"
#include "GHIdentifierMap.h"

class GHXMLObjFactory;
class GHBulletPhysicsObject;
class GHBulletRagdollConstraint;
class GHXMLNode;
class btCollisionShape;
class GHTransform;
class GHXMLNode;
class GHTransformNode;
class GHRagdollGenerator;

class GHRagdollToolNode
{
public:
    GHRagdollToolNode(const GHXMLObjFactory& objFactory,
                      const GHIdentifierMap<int>& enumStore,
                      GHTransformNode* parentSkeleton,
                      GHRagdollGenerator& parentGenerator,
                      GHRagdollGeneratorDescription::NodeDesc& nodeDesc,
                      int index);
    
    GHRagdollToolNode(const GHXMLObjFactory& objFactory,
                      const GHIdentifierMap<int>& enumStore,
                      GHTransformNode* parentSkeleton,
                      GHRagdollGenerator& parentGenerator,
                      GHXMLNode* rigidBodyNode,
                      int index);
    
    void setConstraintNode(GHXMLNode* constraintNode);
    
    void generateConstraint(void);
    void loadConstraint(void);
    
    //To be placed into a parent node for writing to file
    GHXMLNode* getRigidBodyNode(void) { return mRigidBodyNode; }
    GHXMLNode* getConstraintNode(void) { return mConstraintNode; }
    GHBulletPhysicsObject* getPhysicsObject(void) { return mPhysicsObject; }
    GHBulletRagdollConstraint* getConstraint(void) { return mConstraint; }
    int getIndex(void) { return mIndex; }
    
    bool setShape(const char* shape);
    bool setExtent(int index, float value);
    bool setRadius(float radius);
    bool setLimit(const char* limit);
    bool setAxis(const char* axis);
    bool setConstraintType(const char* constraintType);
    
    void applyOffsetChange(const GHTransform& adjustment);
    
    void replaceSkeleton(GHTransformNode* skeleton);
    
    void updatePivotTransform(const char* pivotAttr, const GHTransform& oldTransform, const GHTransform& newTransformInv);


private:
    void loadRigidBodyFromXML(void);
    void createRigidBody(void);
    void generateRigidBodyXML(void);
    GHXMLNode* generateShapeXML(GHTransform& outOffset);
    void reloadRigidBody(void);
    GHXMLNode* getShapeNode(void);
    
private:
    const GHXMLObjFactory& mXMLObjFactory;
    const GHIdentifierMap<int>& mEnumStore;
    GHTransformNode* mParentSkeleton;
    GHRagdollGenerator& mParentGenerator;
    GHRagdollGeneratorDescription::NodeDesc mDesc;
    int mIndex;
    
    //We generate these and share them with the game.
    GHXMLNode* mRigidBodyNode;
    GHXMLNode* mConstraintNode;
    GHBulletPhysicsObject* mPhysicsObject;
    GHBulletRagdollConstraint* mConstraint;
};
