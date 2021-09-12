// Copyright Golden Hammer Software
#include "GHPhysicsCollisionCallback.h"
#include "GHPhysicsCollisionData.h"

void GHPhysicsCollisionCallback::handleCollisionReversed(const GHPhysicsCollisionData& data)
{
    GHPhysicsCollisionData reversedData;
    reverseObjects(data, reversedData);
    onCollision(reversedData);
}

void GHPhysicsCollisionCallback::reverseObjects(const GHPhysicsCollisionData& data, GHPhysicsCollisionData& outData) const
{
    outData.mRHS = data.mLHS;
    outData.mLHS = data.mRHS;
}
