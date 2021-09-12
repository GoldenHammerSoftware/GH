// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include <vector>
#include "GHPhysicsCollisionData.h"
#include "GHMath/GHPoint.h"
#include "GHPlatform/GHPrioritizedList.h"

class GHPhysicsObject;
class GHPhysicsCollisionCallback;

class GHPhysicsSim : public GHController
{
public:
	GHPhysicsSim(void);
    virtual ~GHPhysicsSim() { }
    
    virtual void addToSim(GHPhysicsObject& physicsObject) = 0;
    virtual void removeFromSim(GHPhysicsObject& physicsObject) = 0;
    virtual void pause(void) = 0;
    virtual void unpause(void) = 0;
    virtual void setGravity(const GHPoint3& gravity) = 0;
    virtual void enableDebugDraw(bool enable) = 0;
    
    virtual bool castRay(const GHPoint3& rayStart, const GHPoint3& rayEnd,
                         int collisionLayer,
                         GHPhysicsCollisionData::GHPhysicsCollisionObject& result) const = 0;
    
    virtual bool castRay(const GHPoint3& rayStart, const GHPoint3& rayEnd,
                         int collisionLayer,
                         std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject>& results) const = 0;

	virtual bool findObjectsInSphere(const GHPoint3& sphereCenter, float sphereRadius,
									 int collisionLayer,
									 std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject>& results) const = 0;

    void addCollisionCallback(GHPhysicsCollisionCallback* callback);
    void removeCollisionCallback(GHPhysicsCollisionCallback* callback);
    
    virtual void enableCollisions(int lhsLayer, int rhsLayer, bool collide) = 0;
	virtual void enableCollisionsWithAll(int layer, bool collide) = 0;
    
protected:
    void dispatchCollisionCallbacks(const GHPhysicsCollisionData& data);
    
private:
	GHPrioritizedList<GHPhysicsCollisionCallback*> mCallbacks;
};
