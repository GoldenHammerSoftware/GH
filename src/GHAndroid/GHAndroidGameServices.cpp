// Copyright 2010 Golden Hammer Software
#include "GHAndroidGameServices.h"
#include "GHAppShard.h"
#include "GHAndroidRatingListener.h"
#include "GHPlatform/GHDeletionHandle.h"

GHAndroidGameServices::GHAndroidGameServices(GHJNIMgr& jniMgr, jobject engineInterface,
    GHSystemServices& systemServices,
    GHRenderServices& renderServices,
    GHMessageHandler& appMessageQueue)
    : GHGameServices(systemServices, renderServices, appMessageQueue)
    , mJNIMgr(jniMgr)
    , mEngineInterface(engineInterface)
{
}

void GHAndroidGameServices::initAppShard(GHAppShard& appShard,
                              const GHTimeVal& realTime,
                              const GHTimeVal& animTime,
                              GHRenderGroup& renderGroup,
                              GHPhysicsSim& physicsSim)
{
	GHGameServices::initAppShard(appShard, realTime, animTime, renderGroup, physicsSim);

    GHAndroidRatingListener* ratingListener = new GHAndroidRatingListener(mJNIMgr, mEngineInterface, appShard.mEventMgr);
    appShard.addOwnedItem(new GHTypedDeletionHandle<GHAndroidRatingListener>(ratingListener));
}
