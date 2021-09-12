#pragma once

#include "GHPlatform/android/GHJNIMgr.h"
#include "GHUtils/GHMessageHandler.h"

class GHEventMgr;

class GHAndroidRatingListener : public GHMessageHandler
{
public:
    GHAndroidRatingListener(GHJNIMgr& jniMgr, jobject javaObj,
                            GHEventMgr& eventMgr);
    ~GHAndroidRatingListener(void);

    virtual void handleMessage(const GHMessage& message);
    
private:
    void updateJniFunctions(void);

private:
    GHEventMgr& mEventMgr;
	GHJNIMgr& mJNIMgr;
    jobject mJavaObj;
    jmethodID mRequestMethodId;
};
