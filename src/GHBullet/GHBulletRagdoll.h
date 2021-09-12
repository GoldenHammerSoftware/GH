// Copyright Golden Hammer Software
#pragma once

#include "GHPhysicsObject.h"
#include "GHBulletPhysicsObject.h"
#include <vector>
#include "GHUtils/GHController.h"
#include "GHMath/GHTransform.h"

class GHBulletRagdollConstraint;
class GHBulletPhysicsSim;
class GHControllerMgr;
class GHBulletOffsetModifierTool;

//A physics object representing a ragdoll.
// Contains the physics objects representing the ragdoll's bones
// and the constraints between said bones.
class GHBulletRagdoll : public GHPhysicsObject
{
public:
    GHBulletRagdoll(GHBulletPhysicsSim& physicsSim, GHControllerMgr& controllerMgr, GHTransformNode* skeleton);
    virtual ~GHBulletRagdoll(void);
    virtual void activate(void);
    virtual void deactivate(void);
    
    void replaceBone(int index, GHBulletPhysicsObject* bone);
    void addBone(GHBulletPhysicsObject* bone);
    void addConstraint(GHBulletRagdollConstraint* constraint);
    void removeConstraint(GHBulletRagdollConstraint* constraint);
    
    virtual GHPhysicsObject* clone(GHTransformNode* skeleton);
    
    virtual void getLinearVelocity(GHPoint3& linearVelocity) const;
    virtual void getAngularVelocity(GHPoint3& angularVelocity) const;
    
    virtual void setLinearVelocity(const GHPoint3& linearVelocity);
    virtual void setAngularVelocity(const GHPoint3& angularVelocity);
    
    virtual float getMass(void) const;
    virtual void setFriction(float friction);
    virtual float getFriction(void) const;
    virtual void setRestitution(float restitution);
    virtual float getRestitution(void) const;
    virtual void getCenterOfMass(GHPoint3& outCoM) const;

	virtual void setAngularFactor(const GHPoint3& angularFactor);

    virtual void setCollisionLayer(short collisionLayer);
	virtual void setUserData(void* userData);
    
    //need to override these and pass them to sub-objects
    virtual void applyForce(const GHPoint3& force, const GHPoint3& position);
    virtual void applyImpulse(const GHPoint3& impulse, const GHPoint3& position);
    virtual void applyTorque(const GHPoint3& torque);
    virtual void applyAngularImpulse(const GHPoint3& impulse);
    virtual void resetMotion(void);
    
    void setOffsetModifier(GHBulletOffsetModifierTool* offsetModifier) { mOffsetModifier = offsetModifier; }
    void setTopNode(GHTransformNode* skeleton);
    
private:
    class TopNodeUpdater : public GHController
    {
    public:
        TopNodeUpdater(GHTransformNode* topNode, std::vector<GHBulletPhysicsObject*>& boneList);
        virtual void onActivate(void);
        virtual void onDeactivate(void);
        virtual void update(void);
        void setTopNode(GHTransformNode* topNode);
    private:
        GHTransformNode* mTopNode;
        std::vector<GHBulletPhysicsObject*>& mBoneList;
        GHTransform mRelativeTransform;
    };
    TopNodeUpdater mTopNodeUpdater;
    
private:
    GHBulletPhysicsSim& mPhysicsSim;
    GHControllerMgr& mControllerMgr;
    std::vector<GHBulletPhysicsObject*> mBones;
    std::vector<GHBulletRagdollConstraint*> mConstraints;
    GHBulletOffsetModifierTool* mOffsetModifier;
    bool mIsActive;
};
