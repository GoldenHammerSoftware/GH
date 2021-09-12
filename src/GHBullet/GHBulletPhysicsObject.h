// Copyright Golden Hammer Software
#pragma once

#include "GHPhysicsObject.h"
#include "GHPlatform/GHRefCounted.h"
#include "GHMath/GHTransform.h"

class GHBulletPhysicsSim;
class btRigidBody;
class btCollisionShape;
class GHBulletContactAddedFrictionOverride;

class GHBulletPhysicsObject : public GHPhysicsObject
{
public:
    GHBulletPhysicsObject(GHBulletPhysicsSim& physicsSim);
    ~GHBulletPhysicsObject(void);
    
	bool debugging{ false };
    
    virtual void activate(void);
    virtual void deactivate(void);
    
    virtual GHBulletPhysicsObject* clone(GHTransformNode* skeleton);
    
    void updateFromGame(void);
    void updateToGame(bool physicsTicked);
    
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

	GHBulletContactAddedFrictionOverride* getContactAddedFrictionOverride(void) const { return mContactAddedFrictionOverride; }
    
    //for loading
    void setBulletObject(btRigidBody* rigidBody);
    btRigidBody* getBulletObject(void);
    void setCollisionShape(GHRefCountedType<btCollisionShape>* collisionShape);
    GHRefCountedType<btCollisionShape>* getCollisionShape(void);
    void setOffset(const GHTransform& offset);
    const GHTransform& getOffset(void) const { return mOffset; }
	const GHTransform& getOffsetInv(void) const { return mOffsetInv; }
    //void setCollisionLayer(short collisionLayer) { mCollisionLayer = collisionLayer; }
	void setIsKinematic(bool isKinematic) { mIsKinematic = isKinematic; }

	void setContactAddedFrictionOverride(GHBulletContactAddedFrictionOverride* override) { mContactAddedFrictionOverride = override; }
    
private:
    void applyOffset(GHTransform& outTransform);
    void applyInvOffset(GHTransform& outTransform);
  
private:
    GHBulletPhysicsSim& mSim;
    btRigidBody* mBTObj;
	GHBulletContactAddedFrictionOverride* mContactAddedFrictionOverride{ 0 }; 
    GHTransform mOffset;
    GHTransform mOffsetInv;
    short mCollisionLayer;
	bool mIsKinematic{ false };
    
    //The collision shapes are currently shared betwen GHBulletPhysicsObjects that were
    // cloned from each other. We might want to support sharing between different source objects, too.
    GHRefCountedType<btCollisionShape>* mCollisionShape;
    
    bool mChangeHappened;
	bool mWaitingForPhysicsTickForGameUpdate{ false };
    bool mIsInPhysicsSim{ false };
};
