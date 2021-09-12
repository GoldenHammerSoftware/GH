// Copyright Golden Hammer Software
#pragma once

struct GHPhysicsCollisionData;

class GHPhysicsCollisionCallback
{
public:
    virtual ~GHPhysicsCollisionCallback(void) { }
    
    virtual void onCollision(const GHPhysicsCollisionData& data) = 0;
    
protected:
    void handleCollisionReversed(const GHPhysicsCollisionData& data);
    void reverseObjects(const GHPhysicsCollisionData& data, GHPhysicsCollisionData& outData) const;
};
