#include "GHNullAdFactory.h"
#include "GHNullTransition.h"
#include "GHNullInterstitial.h"
#include "GHInterstitialMgrTransition.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHMessageTypes.h"
#include "GHHouseAd.h"

GHNullAdFactory::GHNullAdFactory(GHMessageHandler& messageHandler)
    : mMessageHandler(messageHandler)
{

}

class GHNullBannerHouseAdTransition : public GHTransition
{
public:
    GHNullBannerHouseAdTransition(GHHouseAd* houseAd, GHPropertyContainer& props,
        const GHIdentifier& displayingAdsProperty)
        : mHouseAd(houseAd)
        , mProps(props)
        , mDisplayingAdsProperty(displayingAdsProperty)
    {
        assert(mHouseAd);
    }
    ~GHNullBannerHouseAdTransition(void) { delete mHouseAd; }

    virtual void activate(void)
    {
        mHouseAd->activate();
        mProps.setProperty(mDisplayingAdsProperty, true);
    }
    virtual void deactivate(void)
    {
        mHouseAd->deactivate();
        mProps.setProperty(mDisplayingAdsProperty, false);
    }

private:
    GHHouseAd* mHouseAd;
    GHPropertyContainer& mProps;
    GHIdentifier mDisplayingAdsProperty;
};

GHTransition* GHNullAdFactory::createBannerAds(const GHTimeVal& timer,
                                               GHPropertyContainer& props,
                                               const GHIdentifier& displayingAdsProperty,
                                               GHHouseAd* houseAd)
{
    if (houseAd) {
        return new GHNullBannerHouseAdTransition(houseAd, props, displayingAdsProperty);
    }
    return new GHNullTransition;
}

GHInterstitialMgrTransition* GHNullAdFactory::createInterstitialAds(void)
{
    GHInterstitialMgrTransition* ret = new GHInterstitialMgrTransition;

    ret->addChild(new GHNullInterstitial(mMessageHandler, GHMessageTypes::INTERSTITIALADFINISHED), 1.0);

    return ret;
}

GHInterstitialMgrTransition* GHNullAdFactory::createRewardInterstitialAds(GHPropertyContainer& props, const GHIdentifier& availabilityProp)
{
    bool setAvailable = true;
    props.setProperty(availabilityProp, setAvailable);
    GHInterstitialMgrTransition* ret = new GHInterstitialMgrTransition;

    ret->addChild(new GHNullInterstitial(mMessageHandler, GHMessageTypes::INTERSTITIALREWARDGRANTED), 1.0);

    return ret;
}
