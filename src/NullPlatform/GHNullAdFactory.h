#pragma once

#include "GHAdFactory.h"

class GHMessageHandler;

//Needs a real message handler so that the null reward interstitial can send the reward granted message
class GHNullAdFactory : public GHAdFactory
{
public:
    GHNullAdFactory(GHMessageHandler& messageHandler);

    virtual GHTransition* createBannerAds(const GHTimeVal& timer,
                                          GHPropertyContainer& props,
                                          const GHIdentifier& displayingAdsProperty,
                                          GHHouseAd* houseAd);

    virtual GHInterstitialMgrTransition* createInterstitialAds(void);

    virtual GHInterstitialMgrTransition* createRewardInterstitialAds(GHPropertyContainer& props, const GHIdentifier& availabilityProp);

private:
    GHMessageHandler& mMessageHandler;
};
