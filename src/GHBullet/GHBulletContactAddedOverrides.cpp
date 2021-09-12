#include "GHBulletTextureLookupFrictionOverride.h"
#include "GHBulletBtInclude.h"
#include "GHMath/GHTransform.h"
#include "GHBulletUtil.h"
#include "GHBulletPhysicsObject.h"

namespace //local namespace
{
	//this is the global callback called from inside bullet whenever a contact point is added (before collision resolution)
	bool onContactAdded(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
	{
		GHBulletPhysicsObject* objA = GHBulletUtil::getGHObjFromBTObj(colObj0Wrap->getCollisionObject());
		GHBulletPhysicsObject* objB = GHBulletUtil::getGHObjFromBTObj(colObj1Wrap->getCollisionObject());

		float frictionA = colObj0Wrap->getCollisionObject()->getFriction();
		float frictionB = colObj1Wrap->getCollisionObject()->getFriction();
		bool frictionNeedsRecalculate = false;
		
		//todo: do we want to handle overriding rolling and spinning friction too? (bullet treats them separately, not sure exactly how)
		if (objA->getContactAddedFrictionOverride()) 
		{
			frictionNeedsRecalculate = true;
			frictionA = objA->getContactAddedFrictionOverride()->getFriction(cp, true);
		}
		if (objB->getContactAddedFrictionOverride())
		{
			frictionNeedsRecalculate = true;
			frictionB = objB->getContactAddedFrictionOverride()->getFriction(cp, false);
		}
	
		if (frictionNeedsRecalculate)
		{
			cp.m_combinedFriction = frictionA*frictionB;
		}
		return true;
	}
}

GHBulletContactAddedOverride::GHBulletContactAddedOverride(void)
{
	registerContactAddedCallback();
}

void GHBulletContactAddedOverride::registerContactAddedCallback(void)
{
	gContactAddedCallback = onContactAdded;
}

