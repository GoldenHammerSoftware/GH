// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAdFactory.h"
#include "GHiOSInterstitialAdHandlingDelegate.h"
#include "GHChartboost.h"
#include "GHiOSAdColonyInitializer.h"

class GHTimeVal;
class GHMessageQueue;
class GHEventMgr;
class GHScreenInfo;
class GHControllerMgr;
class GHMessageHandler;

class GHiOSAdFactory : public GHAdFactory
{
public:
	GHiOSAdFactory(UIView* view,
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
                   NSString* charboostAppId,
                   NSString* chartboostAppSignature,
                   NSString* adColonyAppID,
                   NSString* adColonyInterstitialZoneId,
                   NSString* adColonyBannerZoneId);
	virtual ~GHiOSAdFactory(void);
    
	virtual GHTransition* createBannerAds(const GHTimeVal& timer,
                                          GHPropertyContainer& props,
                                          const GHIdentifier& displayingAdsProperty,
                                          GHHouseAd* houseAd);
    
    virtual GHInterstitialMgrTransition* createInterstitialAds(void);
    virtual GHInterstitialMgrTransition* createRewardInterstitialAds(GHPropertyContainer& props, const GHIdentifier& availabilityProp);
	
    // must be called before createBannerAds.s
    virtual void setHouseBannerAdsOnly(bool val) { mHouseBannerAdsOnly = val; }
    
private:
    GHChartboost mChartboost;
    GHiOSAdColonyInitializer mAdColony;
	UIView* mView;
    UIWindow* mWindow;
    UIViewController* mViewController;
    float mSystemVersion;
    NSString* mAdMobPublisherID;
    NSString* mAdColonyInterstitialZoneId;
    NSString* mAdColonyBannerZoneId;
    GHMessageQueue& mMessageQueue;
    GHEventMgr& mGameThreadEventMgr;
    GHiOSInterstitialAdHandlingDelegate mInterstitialDelegate;
    const GHScreenInfo& mScreenInfo;
    GHControllerMgr& mUIControllerMgr;
    GHMessageHandler& mUIMessageQueue;
    GHEventMgr& mUIThreadEventMgr;
    bool mHouseBannerAdsOnly { false };
};
