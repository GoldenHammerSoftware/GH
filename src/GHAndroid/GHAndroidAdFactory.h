// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAdFactory.h"
#include <jni.h>

class GHControllerMgr;
class GHMessageQueue;
class GHJNIMgr;

class GHAndroidAdFactory : public GHAdFactory
{
public:
	GHAndroidAdFactory(GHJNIMgr& jniMgr, jobject engineInterface,
                       GHControllerMgr& controllerMgr, GHMessageQueue& messageQueue);
	virtual ~GHAndroidAdFactory(void);
    
	virtual GHTransition* createBannerAds(const GHTimeVal& timer,
                                          GHPropertyContainer& props,
                                          const GHIdentifier& displayingAdsProperty,
                                          GHHouseAd* houseAd);
    
    virtual GHInterstitialMgrTransition* createInterstitialAds(void);
	virtual GHInterstitialMgrTransition* createRewardInterstitialAds(GHPropertyContainer& props, const GHIdentifier& availabilityProp);
	
private:
	GHJNIMgr& mJNIMgr;
    jobject mEngineInterface;
    GHControllerMgr& mControllerMgr;
    GHMessageQueue& mMessageQueue;
};
