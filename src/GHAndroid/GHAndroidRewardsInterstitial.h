// Copyright 2010 Golden Hammer Software
#pragma once

#include <jni.h>
#include "GHAdTransition.h"
#include "GHString/GHIdentifier.h"

class GHJNIMgr;
class GHMessageQueue;
class GHPropertyContainer;

class GHAndroidRewardsInterstitial : public GHAdTransition
{
public:
    GHAndroidRewardsInterstitial(GHJNIMgr& jniMgr, jobject javaObj,
                            GHMessageQueue& messageQueue, GHPropertyContainer& props, const GHIdentifier& availabilityProp);
	~GHAndroidRewardsInterstitial(void);
    
    virtual void activate(void);
    virtual void deactivate(void);
    virtual bool hasAd(void) const;

	void onAvailabilityChange(bool available);
    
private:
    GHMessageQueue& mMessageQueue;
	GHPropertyContainer& mProps;
	GHIdentifier mAvailabilityProp;
	GHJNIMgr& mJNIMgr;
    jobject mJavaObj;
    jmethodID mShowAdMethod;
    jmethodID mHideAdMethod;
};
