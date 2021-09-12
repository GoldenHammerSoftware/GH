// Copyright Golden Hammer Software
#pragma once

class btTypedConstraint;
class btRigidBody;

//Owns a btTypedConstraint used to keep two bones in a ragdoll together.
// Also contains any information necessary for cloning the constraint
class GHBulletRagdollConstraint
{
public:
    GHBulletRagdollConstraint(btTypedConstraint* constraint, int parentIndex, int childIndex, bool bodyCollisionDisabled);
    GHBulletRagdollConstraint* clone(btRigidBody* parent, btRigidBody* child) const;
    
    btTypedConstraint* getBulletConstraint(void) { return mConstraint; }
    void getRigidBodyIndices(int& parentIndex, int& childIndex) const { parentIndex = mParentIndex; childIndex = mChildIndex; }
    
    bool bodyCollisionDisabled(void) const { return mBodyCollisionDisabled; }
    
private:
    btTypedConstraint* cloneHinge(btRigidBody* parent, btRigidBody* child) const;
    btTypedConstraint* clonePoint2Point(btRigidBody* parent, btRigidBody* child) const;
    btTypedConstraint* cloneConeTwist(btRigidBody* parent, btRigidBody* child) const;
    btTypedConstraint* clone6DOF(btRigidBody* parent, btRigidBody* child) const;
    
private:
    btTypedConstraint* mConstraint;
    int mParentIndex;
    int mChildIndex;
    bool mBodyCollisionDisabled;
};
