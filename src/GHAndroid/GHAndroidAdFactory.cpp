// Copyright 2010 Golden Hammer Software
#include "GHAndroidAdFactory.h"
#include "GHAndroidAdHandler.h"
#include "GHAndroidInterstitialAd.h"
#include "GHAndroidRewardsInterstitial.h"
#include "GHInterstitialMgrTransition.h"
#include "GHUtils/GHControllerTransition.h"
#include "GHPlatform/android/GHJNIMgr.h"

GHAndroidAdFactory::GHAndroidAdFactory(GHJNIMgr& jniMgr, jobject engineInterface,
                                       GHControllerMgr& controllerMgr, GHMessageQueue& messageQueue)
: mJNIMgr(jniMgr)
, mEngineInterface(engineInterface)
, mControllerMgr(controllerMgr)
, mMessageQueue(messageQueue)
{
}

GHAndroidAdFactory::~GHAndroidAdFactory(void)
{
	
}

GHTransition* GHAndroidAdFactory::createBannerAds(const GHTimeVal& timer,
                                              GHPropertyContainer& props,
                                              const GHIdentifier& displayingAdsProperty,
                                              GHHouseAd* houseAd)
{
	return new GHControllerTransition(mControllerMgr, new GHAndroidAdHandler(mJNIMgr, mEngineInterface, props, displayingAdsProperty, houseAd));
}

GHInterstitialMgrTransition* GHAndroidAdFactory::createInterstitialAds(void)
{
    GHInterstitialMgrTransition* ret = new GHInterstitialMgrTransition;
    GHAndroidInterstitialAd* androidInterstitial = new GHAndroidInterstitialAd(mJNIMgr, mEngineInterface, mMessageQueue);
    ret->addChild(androidInterstitial, 1.0);
    return ret;
}

GHInterstitialMgrTransition* GHAndroidAdFactory::createRewardInterstitialAds(GHPropertyContainer& props, const GHIdentifier& availabilityProp)
{
	GHInterstitialMgrTransition* ret = new GHInterstitialMgrTransition;
	GHAndroidRewardsInterstitial* rewardsInterstitial = new GHAndroidRewardsInterstitial(mJNIMgr, mEngineInterface, mMessageQueue, props, availabilityProp);
	ret->addChild(rewardsInterstitial, 1.0);
	return ret;
}
