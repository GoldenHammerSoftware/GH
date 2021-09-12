// Copyright 2010 Golden Hammer Software
#include "GHChartboostTransition.h"
#include "GHAdHandlingDelegate.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHChartboost.h"

@implementation GHChartboostDelegate
@synthesize mAdID;
@synthesize mGHDelegate;
@synthesize mIsShown;
@synthesize mParent;

-(void) setGHDelegate:(GHAdHandlingDelegate*)ghDelegate andAdID:(void*)adID andParent:(GHChartboostTransition*)parent
{
    mGHDelegate = ghDelegate;
    mAdID = adID;
    mParent = parent;
}

// Called when an interstitial has been received and cached.
- (void)didCacheAd:(CHBCacheEvent *)event error:(nullable CHBCacheError *)error {
   if (error)
   {
       NSLog(@"CB didCacheAd failure for interstitial: %@", [error description]);
       mParent->setHasAd(false);
       if (mIsShown)
       {
           mGHDelegate->adDeactivated(mAdID);
       }
   }
   else
   {
       GHDebugMessage::outputString("CB didCacheAd for Interstitial");
       mParent->setHasAd(true);
   }
}


- (void)didDismissAd:(CHBDismissEvent *)event {
   GHDebugMessage::outputString("CB didDismissAd for Interstitial");
   if (mIsShown) {
       mGHDelegate->adDeactivated(mAdID);
   }
}

- (void)willShowAd:(CHBShowEvent *)event {
   // Pause ongoing processes
   //This should be handled by the game level
    int breakpoint = 7;
    breakpoint = 6;
}

- (void)didShowAd:(CHBShowEvent *)event error:(CHBShowError *)error {
   if (error) {
       GHDebugMessage::outputString("CB didShowAd for Interstitial with error %d", [error code]);
       if (mIsShown) {
           mGHDelegate->adDeactivated(mAdID);
       }
   }
}

-(void)didClickAd:(CHBClickEvent *)event error:(CHBClickError *)error {
    if(error){
        GHDebugMessage::outputString("CB didClickAd for Interstitial with error %d", [error code]);
    }
    else {
        GHDebugMessage::outputString("CB didClickAd for Interstitial");
    }
    if (mIsShown) {
        mGHDelegate->adDeactivated(mAdID);
    }
}

-(void) setShown:(bool)isShown
{
    mIsShown = isShown;
}

@end

GHChartboostTransition::GHChartboostTransition(GHAdHandlingDelegate& ghAdDelegate, GHChartboost& chartboost, UIViewController* viewController)
: mInitListener(*this)
, mHasAd(false)
, mViewController(viewController)
{
    // some of this work might need to happen on the main thread.
    // we expect this transition to be activated on the main thread, but created in any.
    void (^init)(void) = ^(void) {
        mCBDelegate = [[GHChartboostDelegate alloc] init];
        [mCBDelegate setShown:false];
        [mCBDelegate setGHDelegate:&ghAdDelegate andAdID:this andParent:this];
        
        chartboost.initialize(&mInitListener);
    };
    if ([NSThread isMainThread]) {
        init();
    }
    else {
        // dispatch_sync here was causing a deadlock on handle focus changed.
        dispatch_async(dispatch_get_main_queue(), init);
    }
}

void GHChartboostTransition::onInitSuccess(void)
{
    mInterstitial = [[CHBInterstitial alloc] initWithLocation:CBLocationMainMenu delegate:mCBDelegate];
    [mInterstitial cache];
}

void GHChartboostTransition::InitListener::onInitSuccess(void)
{
    mParent.onInitSuccess();
}

void GHChartboostTransition::activate(void)
{
    setHasAd(false);
    dispatch_async(dispatch_get_main_queue(), ^{
        [mCBDelegate setShown:true];
        [mInterstitial showFromViewController:mViewController];
    });
}

void GHChartboostTransition::deactivate(void)
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [mCBDelegate setShown:false];
        [mInterstitial cache];
    });
}
