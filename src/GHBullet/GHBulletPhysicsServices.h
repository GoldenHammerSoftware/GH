// Copyright Golden Hammer Software
#pragma once

#include "Sim/Physics/GHPhysicsServices.h"
#include "GHMath/GHPoint.h"

class GHSystemServices;

class GHBulletPhysicsServices : public GHPhysicsServices
{
public:
    GHBulletPhysicsServices(GHSystemServices& systemServices, const GHPoint3& gravity=GHPoint3(0,0,0));
    
    virtual GHPhysicsSim* createPhysicsSim(const GHTimeVal& timeVal);
    virtual void initAppShard(GHAppShard& appShard, GHPhysicsSim& physicsSim, const GHIdentifierMap<int>& enumStore);
    
private:
    GHSystemServices& mSystemServices;
    GHPoint3 mGravity;
};
