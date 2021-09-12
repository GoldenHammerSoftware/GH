// Copyright Golden Hammer Software
#pragma once

#include "GHPhysicsSim.h"
#include <vector>
#include "GHMath/GHPoint.h"
#include "GHBulletCollisionLayerMgr.h"

class GHTimeVal;
class GHBulletPhysicsObject;
class GHBulletDebugDraw;

class btTypedConstraint;
class btRaycastVehicle;
class btCollisionConfiguration;
class btDynamicsWorld;
class btDispatcher;
class btBroadphaseInterface;
class btConstraintSolver;
class btRigidBody;

class GHBulletPhysicsSim : public GHPhysicsSim
{
public:
    GHBulletPhysicsSim(const GHTimeVal& timeVal, const GHPoint3& gravity);
    ~GHBulletPhysicsSim(void);

    virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);
    
    virtual void addToSim(GHPhysicsObject& physicsObject);
    virtual void removeFromSim(GHPhysicsObject& physicsObject);
    virtual void pause(void);
    virtual void unpause(void);
    virtual void setGravity(const GHPoint3& gravity);
    virtual void enableDebugDraw(bool enable);
    
    virtual bool castRay(const GHPoint3& rayStart, const GHPoint3& rayEnd,
                         int collisionLayer,
                         GHPhysicsCollisionData::GHPhysicsCollisionObject& result) const;
    
    virtual bool castRay(const GHPoint3& rayStart, const GHPoint3& rayEnd,
                         int collisionLayer,
                         std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject>& results) const;

	virtual bool findObjectsInSphere(const GHPoint3& sphereCenter, float sphereRadius,
									 int collisionLayer,
									 std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject>& results) const;
    
    virtual void enableCollisions(int lhsLayer, int rhsLayer, bool collide);
	virtual void enableCollisionsWithAll(int layer, bool collide);

    void addToSim(GHBulletPhysicsObject* ghObj, btRigidBody* rigidBody, short collisionLayer);
    void removeFromSim(GHBulletPhysicsObject* ghObj, btRigidBody* rigidBody);
    void addToSim(btTypedConstraint* constraint, bool disableCollisionBetweenBodies);
    void removeFromSim(btTypedConstraint* constraint);
    void addToSim(btRaycastVehicle* vehicle);
    void removeFromSim(btRaycastVehicle* vehicle);
    
    
    //This is exposed so that we can create things that require
    // access to the world in accordance with the Bullet interface.
    // Please try to use sparingly, and err towards treating GHBulletPhysicsSim
    // as the wrapper for the btDynamicsWorld
    btDynamicsWorld& getBTWorld(void) { return  *mWorld; }
    
    
private:
    void updateToGame(bool physicsTicked);
    void updateFromGame(void);
    void processCollisions(void);
    
private:
    GHBulletCollisionLayerMgr mCollisionLayerMgr;
    const GHTimeVal& mTimeVal;
    btCollisionConfiguration* mCollisionConfiguration;
    btDispatcher* mDispatcher;
    btBroadphaseInterface* mBroadphaseInterface;
    btConstraintSolver* mConstraintSolver;
    btDynamicsWorld* mWorld;
    float mFixedTimestep;
    bool mIsPaused;
    
    GHBulletDebugDraw* mDebugDraw;
    
    typedef std::vector<GHBulletPhysicsObject*> PhysObjList;
    PhysObjList mPhysicsObjectsInWorld;
};
