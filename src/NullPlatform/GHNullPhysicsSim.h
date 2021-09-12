// Copyright Golden Hammer Software
#pragma once

#include "GHPhysicsSim.h"

class GHNullPhysicsSim : public GHPhysicsSim
{
public:

	virtual void update(void) { }
	virtual void onActivate(void) { }
	virtual void onDeactivate(void) { }

	virtual void addToSim(GHPhysicsObject& physicsObject) { }
	virtual void removeFromSim(GHPhysicsObject& physicsObject) { }
	virtual void pause(void) { }
	virtual void unpause(void) { }
	virtual void setGravity(const GHPoint3& gravity) { }
	virtual void enableDebugDraw(bool enable) { }

	virtual bool castRay(const GHPoint3& rayStart, const GHPoint3& rayEnd,
		int collisionLayer,
		GHPhysicsCollisionData::GHPhysicsCollisionObject& result) const { return false; }

	virtual bool castRay(const GHPoint3& rayStart, const GHPoint3& rayEnd,
		int collisionLayer,
		std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject>& results) const { return false; }

	virtual bool findObjectsInSphere(const GHPoint3& sphereCenter, float sphereRadius,
		int collisionLayer,
		std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject>& results) const { return false; }

	virtual void enableCollisions(int lhsLayer, int rhsLayer, bool collide) { }
	virtual void enableCollisionsWithAll(int layer, bool collide) { }
};
