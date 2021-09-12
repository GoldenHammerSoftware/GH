// Copyright 2010 Golden Hammer Software
#include "GHWPAdFactory.h"
#include "GHInstantAbortAd.h"
#include "GHWPBannerAdTransition.h"
#include "GHInterstitialMgrTransition.h"

GHWPAdFactory::GHWPAdFactory(GHMessageHandler& eventMgr,
							 PhoneDirect3DXamlAppComponent::GHWPEventInterface^ eventInterface)
: mInterstitialDelegate(eventMgr)
, mEventInterface(eventInterface)
{
}

GHTransition* GHWPAdFactory::createBannerAds(const GHTimeVal& timer,
                                        GHPropertyContainer& props, 
                                        const GHIdentifier& displayingAdsProperty,
                                        GHHouseAd* houseAd)
{
	return new GHWPBannerAdTransition(mEventInterface, props, displayingAdsProperty);
}

GHInterstitialMgrTransition* GHWPAdFactory::createInterstitialAds(void)
{
	GHInterstitialMgrTransition* ret = new GHInterstitialMgrTransition();
    
    GHInstantAbortAd* fallbackAd = new GHInstantAbortAd(mInterstitialDelegate);
    ret->addChild(fallbackAd, 1);

	return ret;
}
