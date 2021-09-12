// Copyright Golden Hammer Software
#pragma once

class GHTransition;
class GHPropertyContainer;
class GHTimeVal;
class GHHouseAd;
class GHIdentifier;
class GHInterstitialMgrTransition;

class GHAdFactory
{
public:
    virtual ~GHAdFactory(void) { }
	virtual GHTransition* createBannerAds(const GHTimeVal& timer,
                                          GHPropertyContainer& props, 
                                          const GHIdentifier& displayingAdsProperty,
                                          GHHouseAd* houseAd) = 0;
    
    virtual GHInterstitialMgrTransition* createInterstitialAds(void) = 0;

	virtual GHInterstitialMgrTransition* createRewardInterstitialAds(GHPropertyContainer& props, const GHIdentifier& availabilityProp) = 0;
};
