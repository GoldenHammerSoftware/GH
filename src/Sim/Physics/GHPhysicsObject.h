// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHMath/GHPoint.h"

class GHTransformNode;
class GHTransform;
class GHIdentifier;

class GHPhysicsObject
{
public:
    GHPhysicsObject(void)
    : mTransformNode(0)
    , mLastTransformVersion(0)
    , mUserData(0)
    , mTorque(0,0,0)
    , mAngularImpulse(0,0,0)
    , mWantMotionReset(false)
    { }
    
    virtual ~GHPhysicsObject(void) { }
    
    virtual void activate(void) = 0;
    virtual void deactivate(void) = 0;
    
    virtual GHPhysicsObject* clone(GHTransformNode* skeleton) = 0;
    
    void setTransformNode(GHTransformNode* transformNode);
    const GHTransformNode* getTransformNode(void) const { return mTransformNode; }
    GHTransformNode* getTransformNode(void) { return mTransformNode; }
    virtual void setUserData(void* userData) { mUserData = userData; }
    void* getUserData(void) { return mUserData; }
    const void* getUserData(void) const { return mUserData; }
    
    virtual void getLinearVelocity(GHPoint3& linearVelocity) const = 0;
    virtual void getAngularVelocity(GHPoint3& angularVelocity) const = 0;
    
    bool isIdentifiedBy(const GHIdentifier& identifier) const;
    const GHTransform& getTransform(void) const;
    
    //used by game to communicate to physics sim
    virtual void applyForce(const GHPoint3& force, const GHPoint3& position);
    virtual void applyImpulse(const GHPoint3& impulse, const GHPoint3& position);
    virtual void applyTorque(const GHPoint3& torque);
    virtual void applyAngularImpulse(const GHPoint3& impulse);
    virtual void resetMotion(void);
    
    virtual void setLinearVelocity(const GHPoint3& linearVelocity) = 0;
    virtual void setAngularVelocity(const GHPoint3& angularVelocity) = 0;

	virtual float getMass(void) const = 0;
	virtual void setFriction(float friction) = 0;
	virtual float getFriction(void) const = 0;
    virtual void setRestitution(float restitution) = 0;
    virtual float getRestitution(void) const = 0;
    virtual void getCenterOfMass(GHPoint3& outCoM) const = 0;

	virtual void setAngularFactor(const GHPoint3& angularFactor) = 0;

    virtual void setCollisionLayer(short collisionLayer) = 0;
    
    void negateManualPosition(void);
    
protected:
    //used by physics sim
    void setTransform(const GHTransform& transform);
    
    struct Force
    {
        GHPoint3 mForce;
        GHPoint3 mPosition;
        Force(const GHPoint3& force, const GHPoint3& position)
        : mForce(force), mPosition(position)
        { }
        
        Force(const Force& other)
        : mForce(other.mForce), mPosition(other.mPosition)
        { }
        
        Force(void)
        : mForce(0,0,0), mPosition(0,0,0)
        { }
    };
    const std::vector<Force>& getCurrentForces(void) const { return mForces; }
    const std::vector<Force>& getCurrentImpulses(void) const { return mImpulses; }
    const GHPoint3& getCurrentTorque(void) const { return mTorque; }
    const GHPoint3& getCurrentAngularImpulse(void) const { return mAngularImpulse; }
    const GHTransform* getManuallyPositionedTransform(void);
    bool wasMotionReset(void) const { return mWantMotionReset; }
    void resetFrameData(void);
    
private:
    void* mUserData;
    GHTransformNode* mTransformNode;
    bool mWantMotionReset;
    short mLastTransformVersion;
    
    std::vector<Force> mForces;
    std::vector<Force> mImpulses;
    GHPoint3 mTorque;
    GHPoint3 mAngularImpulse;
};
