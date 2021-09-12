// Copyright 2010 Golden Hammer Software
#pragma once

#import <GoogleMobileAds/GoogleMobileAds.h>

class GHMessageHandler;
class GHAdHandlingDelegate;
class GHAdMobWrapper;

@interface GHAdMobDelegate : NSObject<GADBannerViewDelegate> {
	GHMessageHandler* mMessageHandler;
	GHAdHandlingDelegate* mAdHandlingDelegate;
	GHAdMobWrapper* mAdMobWrapper;
	void* mAdID;
    bool mHasEverCachedAnAd;
}

- (void) setAdHandlingDelegate: (GHAdHandlingDelegate*) adDelegate withID:(void*) adID;
- (void) setMessageHandler:(GHMessageHandler*) messageHandler;
- (void) setAdMobWrapper:(GHAdMobWrapper*) adMobWrapper;
- (bool)hasEverCachedAnAd;

// GADBannerViewDelegate
- (void)bannerViewDidReceiveAd:(nonnull GADBannerView *)bannerView;
- (void)bannerView:(nonnull GADBannerView *)bannerView
    didFailToReceiveAdWithError:(nonnull NSError *)error;
- (void)bannerViewWillPresentScreen:(nonnull GADBannerView *)bannerView;
- (void)bannerViewWillDismissScreen:(nonnull GADBannerView *)bannerView;
- (void)bannerViewDidDismissScreen:(nonnull GADBannerView *)bannerView;

@end
