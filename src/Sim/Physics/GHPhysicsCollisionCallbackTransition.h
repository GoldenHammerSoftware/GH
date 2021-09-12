// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include <vector>

class GHPhysicsSim;
class GHPhysicsCollisionCallback;

// A class to add and remove physics callbacks when activated/deactivated.
class GHPhysicsCollisionCallbackTransition : public GHTransition
{
public:
    GHPhysicsCollisionCallbackTransition(GHPhysicsSim& physicsSim,
                                         GHPhysicsCollisionCallback* collisionCallback);
    
    GHPhysicsCollisionCallbackTransition(GHPhysicsSim& physicsSim,
                                         const std::vector<GHPhysicsCollisionCallback*>& callbackList);
    
    ~GHPhysicsCollisionCallbackTransition(void);
    
    virtual void activate(void);
    virtual void deactivate(void);
    
private:
    GHPhysicsSim& mPhysicsSim;
    std::vector<GHPhysicsCollisionCallback*> mCallbackList;
};
