// Copyright 2010 Golden Hammer Software
#include "GHAdMobDelegate.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"
#include "GHAdHandlingDelegate.h"
#include "GHAdMobWrapper.h"
#import <GoogleMobileAds/GADRequestError.h>

@implementation GHAdMobDelegate

- (void) setAdHandlingDelegate: (GHAdHandlingDelegate*) adDelegate withID:(void*) adID
{
	mAdHandlingDelegate = adDelegate;
	mAdID = adID;
    mHasEverCachedAnAd = false;
}

- (void) setMessageHandler:(GHMessageHandler *)messageHandler
{
	mMessageHandler = messageHandler;
}

- (void) setAdMobWrapper:(GHAdMobWrapper*) adMobWrapper
{
	mAdMobWrapper = adMobWrapper;
}

- (void)bannerViewDidReceiveAd:(GADBannerView *)bannerView
{
    mHasEverCachedAnAd = true;
    mAdHandlingDelegate->adIsActive(mAdID);
}

- (void)bannerView:(GADBannerView *)bannerView didFailToReceiveAdWithError:(NSError *)error
{
    mAdHandlingDelegate->adDeactivated(mAdID);
    //NSLog([error description]);
}

- (void)bannerViewWillPresentScreen:(GADBannerView *)bannerView
{
    if (mMessageHandler) { mMessageHandler->handleMessage(GHMessage(GHMessageTypes::PAUSEINTERRUPT)); }
}

- (void)bannerViewDidDismissScreen:(GADBannerView *)bannerView
{
    if (mMessageHandler) { mMessageHandler->handleMessage(GHMessage(GHMessageTypes::UNPAUSEINTERRUPT)); }
	//mAdMobWrapper->resetAdLocation();
}

- (void)bannerViewWillLeaveApplication:(GADBannerView *)bannerView
{
    
}

- (bool)hasEverCachedAnAd
{
    return mHasEverCachedAnAd;
}

@end

