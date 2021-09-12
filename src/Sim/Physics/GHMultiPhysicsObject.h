#pragma once
#include "GHPhysicsObject.h"

//a collection of physics objects that can be handled as a single one
class GHMultiPhysicsObject : public GHPhysicsObject
{
public:
	GHMultiPhysicsObject(size_t numExpectedObjects);
	~GHMultiPhysicsObject(void);

	void addSubObject(GHPhysicsObject* subObject);

	virtual void activate(void);
	virtual void deactivate(void);

	virtual GHPhysicsObject* clone(GHTransformNode* skeleton);

	virtual void getLinearVelocity(GHPoint3& linearVelocity) const;
	virtual void getAngularVelocity(GHPoint3& angularVelocity) const;

	//used by game to communicate to physics sim
	virtual void applyForce(const GHPoint3& force, const GHPoint3& position);
	virtual void applyImpulse(const GHPoint3& impulse, const GHPoint3& position);
	virtual void applyTorque(const GHPoint3& torque);
	virtual void applyAngularImpulse(const GHPoint3& impulse);
	virtual void resetMotion(void);

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

private:
	std::vector<GHPhysicsObject*> mSubObjects;
};

