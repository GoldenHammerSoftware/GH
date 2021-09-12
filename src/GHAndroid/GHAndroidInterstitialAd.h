#pragma once

#include <jni.h>
#include "GHAdTransition.h"

class GHMessageQueue;
class GHJNIMgr;

class GHAndroidInterstitialAd : public GHAdTransition
{
public:
    GHAndroidInterstitialAd(GHJNIMgr& jniMgr, jobject javaObj,
                            GHMessageQueue& messageQueue);
    
    virtual void activate(void);
    virtual void deactivate(void);
    virtual bool hasAd(void) const;
    
private:
    GHMessageQueue& mMessageQueue;
	GHJNIMgr& mJNIMgr;
    jobject mJavaObj;
    jmethodID mShowAdMethod;
    jmethodID mHideAdMethod;
};
