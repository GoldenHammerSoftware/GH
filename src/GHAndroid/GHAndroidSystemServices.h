// Copyright 2010 Golden Hammer Software
#pragma once
// to switch between native and java sound.
//#define GHOPENSL

#include "GHSystemServices.h"
#include <jni.h>
#include <vector>
#include "GHString/GHString.h"

class GHJNIMgr;
class GHAndroidSoundDevice;

class GHAndroidSystemServices : public GHSystemServices
{
public:
	GHAndroidSystemServices(GHJNIMgr& jniMgr, jobject engineInterface, std::vector<GHString>&& saveFileDirectories, jobject jAssetMgr);
    
    virtual void initAppShard(GHAppShard& appShard);
    
private:
#ifdef GHOPENSL
#else
    GHAndroidSoundDevice* mAndroidSoundDevice;
#endif
	GHJNIMgr& mJNIMgr;
    jobject mEngineInterface;
	jobject mAssetMgr;
};
