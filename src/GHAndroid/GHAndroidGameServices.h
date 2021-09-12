// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGameServices.h"
#include "GHPlatform/android/GHJNIMgr.h"

class GHAndroidGameServices : public GHGameServices
{
public:
    GHAndroidGameServices(GHJNIMgr& jniMgr, jobject engineInterface,
        GHSystemServices& systemServices,
        GHRenderServices& renderServices,
        GHMessageHandler& appMessageQueue);

    virtual void initAppShard(GHAppShard& appShard,
        const GHTimeVal& realTime,
        const GHTimeVal& animTime,
        GHRenderGroup& renderGroup,
        GHPhysicsSim& physicsSim);
    
private:
    GHJNIMgr& mJNIMgr;
    jobject mEngineInterface;
};
