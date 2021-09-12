// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAdFactory.h"
#include "GHMetroInterstitialDelegate.h"
#include "GHWPEventInterface.h"

class GHMessageHandler;

class GHWPAdFactory : public GHAdFactory
{
public:
	GHWPAdFactory(GHMessageHandler& eventMgr,
		PhoneDirect3DXamlAppComponent::GHWPEventInterface^ eventInterface);

	virtual GHTransition* createBannerAds(const GHTimeVal& timer,
                                          GHPropertyContainer& props, 
                                          const GHIdentifier& displayingAdsProperty,
                                          GHHouseAd* houseAd);
    
    virtual GHInterstitialMgrTransition* createInterstitialAds(void);
	GHInterstitialMgrTransition* createRewardInterstitialAds(GHPropertyContainer &, const GHIdentifier &) { return 0; }

private:
	GHMetroInterstitialDelegate mInterstitialDelegate;
	PhoneDirect3DXamlAppComponent::GHWPEventInterface^ mEventInterface;
};
