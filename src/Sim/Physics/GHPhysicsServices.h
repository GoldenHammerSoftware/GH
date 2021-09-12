// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifierMap.h"

class GHAppShard;
class GHPhysicsSim;
class GHTimeVal;

class GHPhysicsServices
{
public:
    virtual ~GHPhysicsServices(void) { }
    
public:
    virtual GHPhysicsSim* createPhysicsSim(const GHTimeVal& timeVal) = 0;
    virtual void initAppShard(GHAppShard& appShard, GHPhysicsSim& physicsSim, const GHIdentifierMap<int>& enumStore);
};
