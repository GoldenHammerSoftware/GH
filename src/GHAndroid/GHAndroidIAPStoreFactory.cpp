// Copyright 2010 Golden Hammer Software
#include "GHAndroidIAPStoreFactory.h"
#include "GHAndroidIAPStore.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"
#include "GHPlatform/android/GHJNIMgr.h"

GHAndroidIAPStoreFactory::GHAndroidIAPStoreFactory(GHJNIMgr& jniMgr, jobject engineInterface, GHEventMgr& gameThreadEventMgr, GHMessageQueue& gameThreadMessageQueue)
: mJNIMgr(jniMgr)
, mEngineInterface(engineInterface)
, mGameThreadEventMgr(gameThreadEventMgr)
, mGameThreadMessageQueue(gameThreadMessageQueue)
{
   
}

GHIAPStore* GHAndroidIAPStoreFactory::createIAPStore(const char* configFilename,
                                                 const GHXMLSerializer& xmlSerializer) const
{
    GHAndroidIAPStore* ret = new GHAndroidIAPStore(mJNIMgr, mEngineInterface, mGameThreadEventMgr, mGameThreadMessageQueue);
    return ret;
}
