// Copyright Golden Hammer Software
#include "GHPhysicsSim.h"
#include <algorithm>
#include "GHPhysicsCollisionCallback.h"

GHPhysicsSim::GHPhysicsSim(void)
: mCallbacks(false)
{
}

void GHPhysicsSim::addCollisionCallback(GHPhysicsCollisionCallback* callback)
{
	mCallbacks.addEntry(callback, 0);
}

void GHPhysicsSim::removeCollisionCallback(GHPhysicsCollisionCallback* callback)
{
	mCallbacks.removeEntry(callback);
}

void GHPhysicsSim::dispatchCollisionCallbacks(const GHPhysicsCollisionData& data)
{
	mCallbacks.startIteration();
	size_t numCallbacks = mCallbacks.getNumEntries();
	for (size_t i = 0; i < numCallbacks; ++i)
	{
		GHPhysicsCollisionCallback* currCon = mCallbacks.getEntry(i);
		if (!mCallbacks.isInChangeList(currCon))
		{
			currCon->onCollision(data);
		}
	}
	mCallbacks.endIteration();
}
