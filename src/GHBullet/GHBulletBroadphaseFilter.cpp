// Copyright Golden Hammer Software
#include "GHBulletBroadphaseFilter.h"
#include "GHBulletPhysicsObject.h"

bool GHBulletBroadphaseFilter::needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
{
    //btBroadphaseProxy::CollisionFilterGroups
    return true;
}
