// Copyright Golden Hammer Software
#pragma once

class GHBulletRagdollConstraint;
class btTypedConstraint;
class GHXMLNode;
class GHBulletPhysicsObject;
class btRigidBody;
class btTransform;
class btVector3;

//Encapsulates the loading code for ragdoll constraints.
// Called by GHBulletRagdollXMLLoader.
//This class exists because the constraint loading code may get
// verbose due to the variations in constraint type.
class GHBulletRagdollConstraintXMLLoader
{
public:
    GHBulletRagdollConstraint* loadConstraint(const GHXMLNode& xmlNode, GHBulletPhysicsObject* parent, int parentIndex, GHBulletPhysicsObject* child, int childIndex) const;


private:
    btTypedConstraint* loadBulletConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const;
    btTypedConstraint* loadHingeConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const;
    btTypedConstraint* loadPoint2PointConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const;
    btTypedConstraint* loadConeTwistConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const;
    btTypedConstraint* loadSliderConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const;
    btTypedConstraint* load6DOFConstraint(const GHXMLNode& xmlNode, btRigidBody* parent, btRigidBody* child) const;
    
    void readFrame(const GHXMLNode& xmlNode, const char* transAttr, const char* posAttr, btTransform& outTrans) const;
    void readBTVector(const GHXMLNode& xmlNode, const char* attr, btVector3& outVector) const;
};
