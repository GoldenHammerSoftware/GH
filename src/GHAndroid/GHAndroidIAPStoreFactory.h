// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHIAPStoreFactory.h"
#include <jni.h>

class GHEventMgr;
class GHMessageQueue;
class GHJNIMgr;

// interface to java iap.
class GHAndroidIAPStoreFactory : public GHIAPStoreFactory
{
public:   
    GHAndroidIAPStoreFactory(GHJNIMgr& jniMgr, jobject engineInterface, GHEventMgr& gameThreadEventMgr, GHMessageQueue& gameThreadMessageQueue);
    virtual GHIAPStore* createIAPStore(const char* configFilename,
                                       const GHXMLSerializer& xmlSerializer) const;
    
private:
	GHJNIMgr& mJNIMgr;
    jobject mEngineInterface;
    GHEventMgr& mGameThreadEventMgr;
    GHMessageQueue& mGameThreadMessageQueue;
};
