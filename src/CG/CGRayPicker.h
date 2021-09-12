// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPoint.h"
#include <vector>
#include "GHScreenPosUnprojector.h"

class GHPhysicsSim;
class GHEntity;
class GHPhysicsObject;

// class for colliding rays from the screen to the world.
class CGRayPicker
{
public:
    CGRayPicker(const GHViewInfo& viewInfo, GHPhysicsSim& physicsSim);
    
    bool findGroundPos(const GHPoint2& screenPos, GHPoint3& worldPos) const;
    void findEntities(const GHPoint2& screenPos, std::vector<GHEntity*>& ret) const;
    
    void setGround(const GHPhysicsObject* groundPhysics) { mGroundPhysics = groundPhysics; }
    
private:
    GHScreenPosUnprojector mUnprojector;
    GHPhysicsSim& mPhysicsSim;
    const GHPhysicsObject* mGroundPhysics;
};
