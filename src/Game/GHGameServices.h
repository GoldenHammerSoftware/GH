// Copyright Golden Hammer Software
#pragma once

class GHTimeVal;
class GHSystemServices;
class GHRenderServices;
class GHRenderGroup;
class GHPhysicsSim;
class GHAppShard;
class GHMessageHandler;

class GHGameServices
{
public:
    GHGameServices(GHSystemServices& systemServices, 
                   GHRenderServices& renderServices,
                   GHMessageHandler& appMessageQueue);
    virtual ~GHGameServices(void) {}
    
    virtual void initAppShard(GHAppShard& appShard,
                              const GHTimeVal& realTime,
                              const GHTimeVal& animTime,
                              GHRenderGroup& renderGroup,
                              GHPhysicsSim& physicsSim);
    
private:
    GHSystemServices& mSystemServices; 
    GHRenderServices& mRenderServices;
    GHMessageHandler& mAppMessageQueue;
};
