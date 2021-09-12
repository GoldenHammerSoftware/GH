// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAdFactory.h"
#include "GHMetroInterstitialDelegate.h"

class GHEventMgr;
class GHScreenInfo;
class GHControllerMgr;

class GHMetroAdFactory : public GHAdFactory
{
public:
	GHMetroAdFactory(Microsoft::Advertising::WinRT::UI::AdControl^ adXamlControl,
		GHEventMgr& eventMgr, const GHScreenInfo& screenInfo,
		GHControllerMgr& uiControllerMgr);

	virtual GHTransition* createBannerAds(const GHTimeVal& timer,
                                          GHPropertyContainer& props, 
                                          const GHIdentifier& displayingAdsProperty,
                                          GHHouseAd* houseAd);
    
    virtual GHInterstitialMgrTransition* createInterstitialAds(void);

	virtual GHInterstitialMgrTransition* createRewardInterstitialAds(GHPropertyContainer& props, const GHIdentifier& availabilityProp);

private:
	Microsoft::Advertising::WinRT::UI::AdControl^ mAdXamlControl;
	Windows::UI::Core::CoreDispatcher^ mUIDispatcher;
	GHEventMgr& mEventMgr;
	const GHScreenInfo& mScreenInfo;
	GHMetroInterstitialDelegate mInterstitialDelegate;
	GHControllerMgr& mUIControllerMgr;
};
