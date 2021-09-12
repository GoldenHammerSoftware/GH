// Copyright Golden Hammer Software
#include "GHPhysicsCollisionCallbackTransition.h"
#include "GHPhysicsSim.h"
#include "GHPhysicsCollisionCallback.h"
#include <stddef.h>

GHPhysicsCollisionCallbackTransition::GHPhysicsCollisionCallbackTransition(GHPhysicsSim& physicsSim,
                                                                           GHPhysicsCollisionCallback* collisionCallback)
: mPhysicsSim(physicsSim)
{
    mCallbackList.push_back(collisionCallback);
}

GHPhysicsCollisionCallbackTransition::GHPhysicsCollisionCallbackTransition(GHPhysicsSim& physicsSim,
                                                                           const std::vector<GHPhysicsCollisionCallback*>& callbackList)
: mPhysicsSim(physicsSim)
, mCallbackList(callbackList)
{
    
}

GHPhysicsCollisionCallbackTransition::~GHPhysicsCollisionCallbackTransition(void)
{
    size_t numCallbacks = mCallbackList.size();
    for (size_t i = 0; i < numCallbacks; ++i)
    {
        delete mCallbackList[i];
    }
}

void GHPhysicsCollisionCallbackTransition::activate(void)
{
    size_t numCallbacks = mCallbackList.size();
    for (size_t i = 0; i < numCallbacks; ++i)
    {
        mPhysicsSim.addCollisionCallback(mCallbackList[i]);
    }
}

void GHPhysicsCollisionCallbackTransition::deactivate(void)
{
    int numCallbacks = (int)mCallbackList.size();
    for (int i = numCallbacks - 1; i >= 0; --i)
    {
        mPhysicsSim.removeCollisionCallback(mCallbackList[i]);
    }
}
