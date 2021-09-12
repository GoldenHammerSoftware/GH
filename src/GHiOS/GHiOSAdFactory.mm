// Copyright 2010 Golden Hammer Software
#import <UIKit/UIKit.h>
#include "GHiOSAdFactory.h"
#include "GHiOSAdHandler.h"
#include "GHAdMobWrapper.h"
#include "GHiOSHouseAdWrapper.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHInterstitialMgrTransition.h"
#include "GHThreadUtil.h"
#include "GHUtils/GHControllerTransition.h"

#include "GHInstantAbortAd.h"
#include "GHChartboostTransition.h" //Interstitials
#include "GHChartboostWrapper.h" //Banners
#include "GHiOSAdColonyRewardsInterstitial.h"
#include "GHiOSAdColonyBanner.h"

//#define HOUSE_BANNER_ADS_ONLY 1
//#define HOUSE_INTERSTITIAL_ADS_ONLY 1

GHiOSAdFactory::GHiOSAdFactory(UIView* view,
                               UIWindow* window,
                               UIViewController* viewController,
                               float systemVersion,
                               GHMessageQueue& messageQueue,
                               GHEventMgr& gameThreadEventMgr,
                               const GHScreenInfo& screenInfo,
                               GHControllerMgr& uiControllerMgr,
                               GHMessageHandler& uiMessageQueue,
                               GHEventMgr& uiThreadEventMgr,
                               NSString* adMobPublisherID,
                               NSString* chartboostAppId,
                               NSString* chartboostAppSignature,
                               NSString* adColonyAppID,
                               NSString* adColonyInterstitialZoneId,
                               NSString* adColonyBannerZoneId)
: mChartboost(chartboostAppId, chartboostAppSignature)
, mAdColony(adColonyAppID, @[adColonyInterstitialZoneId, adColonyBannerZoneId])
, mView(view)
, mWindow(window)
, mViewController(viewController)
, mSystemVersion(systemVersion)
, mAdMobPublisherID(adMobPublisherID)
, mMessageQueue(messageQueue)
, mInterstitialDelegate(messageQueue)
, mGameThreadEventMgr(gameThreadEventMgr)
, mScreenInfo(screenInfo)
, mUIControllerMgr(uiControllerMgr)
, mUIMessageQueue(uiMessageQueue)
, mUIThreadEventMgr(uiThreadEventMgr)
, mAdColonyInterstitialZoneId(adColonyInterstitialZoneId)
, mAdColonyBannerZoneId(adColonyBannerZoneId)
{
}

GHiOSAdFactory::~GHiOSAdFactory(void)
{
	
}

GHTransition* GHiOSAdFactory::createBannerAds(const GHTimeVal& timer,
                                              GHPropertyContainer& props,
                                              const GHIdentifier& displayingAdsProperty,
                                              GHHouseAd* houseAd)
{
	GHiOSAdHandler* adController = new GHiOSAdHandler(timer,
                                             props, 
                                             displayingAdsProperty,
                                             mGameThreadEventMgr,
                                             mScreenInfo);
    
#ifndef HOUSE_BANNER_ADS_ONLY
    if (!mHouseBannerAdsOnly)
    {
        //adController->addWrapper(new GHChartboostWrapper(mView, mMessageQueue, mChartboost));
        adController->addWrapper(new GHiOSAdColonyBanner(mView, mMessageQueue, mAdColony, mAdColonyBannerZoneId));
        adController->addWrapper(new GHAdMobWrapper(mView, mMessageQueue, mAdMobPublisherID));
    }
#endif
    
    if (houseAd)
    {
        adController->addWrapper(new GHiOSHouseAdWrapper(houseAd));
    }

	GHControllerTransition* adTrans = new GHControllerTransition(mUIControllerMgr, adController);
    GHTransition* wrapperTrans = GHThreadUtil::createMessageWrapperTransition(*adTrans, mUIMessageQueue);
    // todo: not leak adTrans
	return wrapperTrans;
}

GHInterstitialMgrTransition* GHiOSAdFactory::createInterstitialAds(void)
{
    GHInterstitialMgrTransition* ret = new GHInterstitialMgrTransition();
    
    /*
    // if we decide not to support any ads, uncomment this block
    GHInstantAbortAd* fallbackAd = new GHInstantAbortAd(mInterstitialDelegate);
    ret->addChild(fallbackAd, 1);
    return ret;
    //*/
    
#ifndef HOUSE_INTERSTITIAL_ADS_ONLY
    GHChartboostTransition* cbAd = new GHChartboostTransition(mInterstitialDelegate, mChartboost, mViewController);
    ret->addChild(cbAd, 0.5);
#endif
    
    return ret;
}

GHInterstitialMgrTransition* GHiOSAdFactory::createRewardInterstitialAds(GHPropertyContainer& props, const GHIdentifier& availabilityProp)
{
    GHInterstitialMgrTransition* ret = new GHInterstitialMgrTransition();

    GHiOSAdColonyRewardsInterstitial* adColony = new GHiOSAdColonyRewardsInterstitial(mMessageQueue, mInterstitialDelegate, props, availabilityProp, mAdColony, mAdColonyInterstitialZoneId, mViewController);
    ret->addChild(adColony, 1);
    
    return ret;
}

