// Copyright 2010 Golden Hammer Software
#include "GHiOSAdColonyRewardsInterstitial.h"

#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHPropertyContainer.h"

@implementation GHAdColonyInterstitialDelegate
@synthesize mParent;

-(void) setGHParent:(GHiOSAdColonyRewardsInterstitial*)parent
{
    mParent = parent;
}

- (void)adColonyInterstitialDidLoad:(AdColonyInterstitial * _Nonnull)interstitial
{
    mParent->handleAdChange(true, interstitial, NULL);
}
- (void)adColonyInterstitialDidFailToLoad:(AdColonyAdRequestError * _Nonnull)error
{
    mParent->handleAdChange(false, NULL, error);
}
- (void)adColonyInterstitialDidClose:(AdColonyInterstitial * _Nonnull)interstitial
{
    mParent->handleAdChange(false, NULL, NULL);
}
- (void)adColonyInterstitialExpired:(AdColonyInterstitial * _Nonnull)interstitial
{
    mParent->handleAdChange(false, NULL, NULL);
}
@end

GHiOSAdColonyRewardsInterstitial::GHiOSAdColonyRewardsInterstitial(GHMessageQueue& messageQueue,
                                                                   GHAdHandlingDelegate& adHandlingDelegate,
                                                                   GHPropertyContainer& props,
                                                                   const GHIdentifier& availabilityProp,
                                                                   GHiOSAdColonyInitializer& initializer,
                                                                   NSString* zoneID,
                                                                   UIViewController* viewController)
: mInitListener(*this)
, mMessageQueue(messageQueue)
, mAdHandlingDelegate(adHandlingDelegate)
, mInitializer(initializer)
, mProps(props)
, mAvailabilityProp(availabilityProp)
, mZoneID(zoneID)
, mAd(0)
, mTopViewController(viewController)
{
    initializer.initialize(&mInitListener, zoneID);
    mAdColonyDelegate = [[GHAdColonyInterstitialDelegate alloc] init];
    [mAdColonyDelegate setGHParent:this];
}

GHiOSAdColonyRewardsInterstitial::~GHiOSAdColonyRewardsInterstitial(void)
{
    
}

void GHiOSAdColonyRewardsInterstitial::handleAdChange(bool adIsReady, AdColonyInterstitial* ad, AdColonyAdRequestError* error)
{
    if (adIsReady)
    {
        mAd = ad;
        onAvailabilityChange(true);
    }
    else
    {
        mAd = 0;
        onAvailabilityChange(false);
        if (error == NULL)
        {
            requestInterstitial();
        }
        else
        {
            NSLog(@"Trick Shot Bowling: Request failed with error: %@ and suggestion: %@", [error localizedDescription], [error localizedRecoverySuggestion]);
        }
    }
}

void GHiOSAdColonyRewardsInterstitial::requestInterstitial(void)
{
    //Request an interstitial ad from AdColony
    [AdColony requestInterstitialInZone:mZoneID options:nil andDelegate: mAdColonyDelegate];
}

void GHiOSAdColonyRewardsInterstitial::handleZoneReturned(AdColonyZone* zone)
{
    // set up the reward callback.
    zone.reward = ^(BOOL success, NSString *name, int amount)
    {
        if (success)
        {
            onReward(true);
        }
    };

    
    //AdColony has finished configuring, so let's request an interstitial ad
    requestInterstitial();
    
    //If the application has been inactive for a while, our ad might have expired so let's add a check for a nil ad object
    //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onBecameActive) name:UIApplicationDidBecomeActiveNotification object:nil];
}

void GHiOSAdColonyRewardsInterstitial::activate(void)
{
    if (hasAd())
    {
        [mAd showWithPresentingViewController:mTopViewController];
    }
    else
    {
        int brkpt = 1;
        brkpt++;
    }
}

void GHiOSAdColonyRewardsInterstitial::deactivate(void)
{
    
}

bool GHiOSAdColonyRewardsInterstitial::hasAd(void) const
{
    return (mAd != 0 && !mAd.expired);
}

void GHiOSAdColonyRewardsInterstitial::onAvailabilityChange(bool available)
{
    mProps.setProperty(mAvailabilityProp, available);
}

void GHiOSAdColonyRewardsInterstitial::onReward(bool success)
{
    mMessageQueue.handleMessage(GHMessage(GHMessageTypes::INTERSTITIALREWARDGRANTED));
}

void GHiOSAdColonyRewardsInterstitial::InitListener::onZoneReturned(AdColonyZone* zone)
{
    mParent.handleZoneReturned(zone);
}

