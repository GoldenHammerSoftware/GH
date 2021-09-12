#include "GHMultiPhysicsObject.h"
#include "GHMath/GHFloat.h"

GHMultiPhysicsObject::GHMultiPhysicsObject(size_t numExpectedObjects)
{
	mSubObjects.reserve(numExpectedObjects);
}

GHMultiPhysicsObject::~GHMultiPhysicsObject(void)
{
	int numObjects = static_cast<int>(mSubObjects.size());
	for (int i = numObjects - 1; i >= 0; --i)
	{
		delete mSubObjects[i];
	}
}

void GHMultiPhysicsObject::addSubObject(GHPhysicsObject* subObject)
{
	mSubObjects.push_back(subObject);
}

void GHMultiPhysicsObject::activate(void)
{
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		mSubObjects[i]->activate();
	}
}

void GHMultiPhysicsObject::deactivate(void)
{
	int numObjects = static_cast<int>(mSubObjects.size());
	for (int i = numObjects - 1; i >= 0; --i)
	{
		mSubObjects[i]->deactivate();
	}
}

GHPhysicsObject* GHMultiPhysicsObject::clone(GHTransformNode* skeleton)
{
	unsigned int numObjects = (unsigned int)mSubObjects.size();
	GHMultiPhysicsObject* myClone = new GHMultiPhysicsObject(numObjects);
	for (unsigned int i = 0; i < numObjects; ++i)
	{
		myClone->addSubObject(mSubObjects[i]->clone(skeleton));
	}
	return myClone;
}

void GHMultiPhysicsObject::getLinearVelocity(GHPoint3& linearVelocity) const
{
	linearVelocity.setCoords(0, 0, 0);
	size_t numObjects = mSubObjects.size();
	if (numObjects == 0)
	{
		return;
	}
	for (size_t i = 0; i < numObjects; ++i)
	{
		GHPoint3 objVel;
		mSubObjects[i]->getLinearVelocity(objVel);
		linearVelocity += objVel;
	}
	linearVelocity /= static_cast<float>(numObjects);
}

void GHMultiPhysicsObject::getAngularVelocity(GHPoint3& angularVelocity) const
{
	angularVelocity.setCoords(0, 0, 0);
	size_t numObjects = mSubObjects.size();
	if (numObjects == 0)
	{
		return;
	}
	for (size_t i = 0; i < numObjects; ++i)
	{
		GHPoint3 objVel;
		mSubObjects[i]->getLinearVelocity(objVel);
		angularVelocity += objVel;
	}
	angularVelocity /= static_cast<float>(numObjects);
}

void GHMultiPhysicsObject::applyForce(const GHPoint3& force, const GHPoint3& position)
{
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		GHPoint3 objVel;
		mSubObjects[i]->applyForce(force, position);
	}
}

void GHMultiPhysicsObject::applyImpulse(const GHPoint3& impulse, const GHPoint3& position)
{
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		GHPoint3 objVel;
		mSubObjects[i]->applyImpulse(impulse, position);
	}
}

void GHMultiPhysicsObject::applyTorque(const GHPoint3& torque)
{
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		GHPoint3 objVel;
		mSubObjects[i]->applyTorque(torque);
	}
}

void GHMultiPhysicsObject::applyAngularImpulse(const GHPoint3& impulse)
{
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		GHPoint3 objVel;
		mSubObjects[i]->applyAngularImpulse(impulse);
	}
}
void GHMultiPhysicsObject::resetMotion(void)
{
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		GHPoint3 objVel;
		mSubObjects[i]->resetMotion();
	}
}

void GHMultiPhysicsObject::setLinearVelocity(const GHPoint3& linearVelocity)
{
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		GHPoint3 objVel;
		mSubObjects[i]->setLinearVelocity(linearVelocity);
	}
}

void GHMultiPhysicsObject::setAngularVelocity(const GHPoint3& angularVelocity)
{
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		GHPoint3 objVel;
		mSubObjects[i]->setAngularVelocity(angularVelocity);
	}
}

float GHMultiPhysicsObject::getMass(void) const
{
	float totalMass = 0;
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		totalMass += mSubObjects[i]->getMass();
	}
	return totalMass;
}
void GHMultiPhysicsObject::setFriction(float friction)
{
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		mSubObjects[i]->setFriction(friction);
	}
}

float GHMultiPhysicsObject::getFriction(void) const
{
	float averageFriction = 0;
	size_t numObjects = mSubObjects.size();
	if (numObjects == 0)
	{
		return averageFriction;
	}
	for (size_t i = 0; i < numObjects; ++i)
	{
		averageFriction += mSubObjects[i]->getFriction();
	}
	averageFriction /= static_cast<float>(numObjects);
	return averageFriction;
}

void GHMultiPhysicsObject::setRestitution(float restitution)
{
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		mSubObjects[i]->setRestitution(restitution);
	}
}

float GHMultiPhysicsObject::getRestitution(void) const
{
	float averageRestitution = 0;
	size_t numObjects = mSubObjects.size();
	if (numObjects == 0)
	{
		return averageRestitution;
	}
	for (size_t i = 0; i < numObjects; ++i)
	{
		averageRestitution += mSubObjects[i]->getRestitution();
	}
	averageRestitution /= static_cast<float>(numObjects);
	return averageRestitution;
}

void GHMultiPhysicsObject::getCenterOfMass(GHPoint3& outCoM) const
{
	size_t numObjects = mSubObjects.size();
	if (!numObjects) {
		outCoM.setCoords(0, 0, 0);
		return;
	}

	bool doDivideByMass = true;
	GHPoint3 comWithMass(0, 0, 0);
	GHPoint3 comWithoutMass(0, 0, 0);
	float totalMass = 0;
	for (size_t i = 0; i < numObjects; ++i)
	{
		const auto subObject = mSubObjects[i];

		float mass = subObject->getMass();

		//If any object has 0 mass, then just take the average CoM assuming equal mass for all objects
		if (!GHFloat::isZero(mass)) {
			doDivideByMass = false;
		}

		GHPoint3 com;
		subObject->getCenterOfMass(com);

		comWithoutMass += com;
		com *= mass;
		comWithMass += com;
		totalMass += mass;
	}

	if (doDivideByMass)
	{
		comWithMass /= totalMass;
		outCoM = comWithMass;
	}
	else
	{
		comWithoutMass /= (float)numObjects;
		outCoM = comWithoutMass;
	}
}

void GHMultiPhysicsObject::setAngularFactor(const GHPoint3& angularFactor)
{
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		mSubObjects[i]->setAngularFactor(angularFactor);
	}
}

void GHMultiPhysicsObject::setCollisionLayer(short collisionLayer)
{
	assert(false && "GHMultiPhysicsObject::setCollisionLayer unimplemented and untested (note - GHBulletPhysicsObject adds/removes from sim. this might cause issues)");
}

void GHMultiPhysicsObject::setUserData(void* userData)
{
	GHPhysicsObject::setUserData(userData);
	size_t numObjects = mSubObjects.size();
	for (size_t i = 0; i < numObjects; ++i)
	{
		mSubObjects[i]->setUserData(userData);
	}
}
