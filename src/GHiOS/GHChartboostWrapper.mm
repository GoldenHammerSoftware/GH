#include "GHChartboostWrapper.h"
#include "GHAdHandlingDelegate.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHMessageTypes.h"

@implementation GHChartboostBannerDelegate
@synthesize mAdID;
@synthesize mGHDelegate;
@synthesize mParent;
@synthesize mHasEverCachedAnAd;

-(void) setGHDelegate:(GHAdHandlingDelegate*)ghDelegate andAdID:(void*)adID andParent:(GHChartboostWrapper*)parent
{
    mHasEverCachedAnAd = false;
    mGHDelegate = ghDelegate;
    mAdID = adID;
    mParent = parent;
}

- (void) setMessageHandler:(GHMessageHandler *)messageHandler
{
    mMessageHandler = messageHandler;
}

// Called when an interstitial has been received and cached.
- (void)didCacheAd:(CHBCacheEvent *)event error:(nullable CHBCacheError *)error {
   if (error)
   {
       NSLog(@"CB didCacheAd failure for banner: %@", [error description]);
       
       //No need to deactivate if we have an ad cached:
       // This might just be a "no fill" error on refresh, but we can
       // continue showing the previously loaded ad.
       if(!mHasEverCachedAnAd) {
           mGHDelegate->adDeactivated(mAdID);
       }
   }
   else
   {
       GHDebugMessage::outputString("CB didCacheAd for banner");
       mHasEverCachedAnAd = true;
   }
}

- (void)willShowAd:(CHBShowEvent *)event {
    GHDebugMessage::outputString("CB willShowAd for banner");
    mGHDelegate->adIsActive(mAdID);
}

-(void)didClickAd:(CHBClickEvent*)event error:(nullable CHBClickError*) error{
    if(error){
        NSLog(@"CB didClickAd failure for banner: %@", [error description]);
    }
    else {
        if (mMessageHandler) { mMessageHandler->handleMessage(GHMessage(GHMessageTypes::PAUSEINTERRUPT)); }
    }
}

-(void)didFinishHandlingClick:(CHBClickEvent *)event error:(CHBClickError *)error {
    if(error) {
        NSLog(@"CB didFinishHandlingClick failure for banner: %@", [error description]);
    }
    if (mMessageHandler) { mMessageHandler->handleMessage(GHMessage(GHMessageTypes::PAUSEINTERRUPT)); }
}

@end

GHChartboostWrapper::GHChartboostWrapper(UIView* view, GHMessageHandler& eventMgr, GHChartboost& chartboost)
: mInitListener(*this)
, mSuperView(view)
, mEventMgr(eventMgr)
, mChartboost(chartboost)
{
    
}

GHChartboostWrapper::~GHChartboostWrapper(void)
{
    
}

void GHChartboostWrapper::initialize(GHAdHandlingDelegate* adHandlingDelegate, int adID)
{
    mCBDelegate = [GHChartboostBannerDelegate alloc];
    [mCBDelegate setGHDelegate:adHandlingDelegate andAdID:this andParent:this];
    
    mGHDelegate = adHandlingDelegate;
    mChartboost.initialize(&mInitListener);
}

void GHChartboostWrapper::setOrientation(bool isPortrait)
{
    
}

void GHChartboostWrapper::update(const GHTimeVal& timeVal)
{
    
}

void GHChartboostWrapper::setActive(bool active)
{
    if (active && !mIsActive) {
        [mSuperView addSubview:mBanner];
        [mBanner setAutomaticallyRefreshesContent:true];
        positionBannerInSafeArea();
        mForceRefresh = true;
        mIsActive = true;
        if (mGHDelegate && mCBDelegate.mHasEverCachedAnAd) { mGHDelegate->adIsActive(this); }
    }
    else if (!active && mIsActive) {
        [mBanner setAutomaticallyRefreshesContent:false];
        [mBanner removeFromSuperview];
        mForceRefresh = false;
        mIsActive = false;
        if (mGHDelegate) { mGHDelegate->adDeactivated(this); }
    }
}

void GHChartboostWrapper::onInitSuccess(void)
{
    mBanner = [[CHBBanner alloc] initWithSize:CHBBannerSizeStandard location:CBLocationDefault delegate:mCBDelegate];
    mBanner.translatesAutoresizingMaskIntoConstraints = NO;
    [mBanner showFromViewController:nil];
}

void GHChartboostWrapper::InitListener::onInitSuccess(void)
{
    mParent.onInitSuccess();
}

void GHChartboostWrapper::positionBannerInSafeArea(void)
{
    if (@available(ios 11.0, *))
    {
        UILayoutGuide* guide = mSuperView.safeAreaLayoutGuide;
        if (0)
        {
            // https://answers.chartboost.com/en-us/child_article/using-ios-banner
            // the docs seem to say this is how we should do it.
            // we were unable to test this change with no fill.
            [NSLayoutConstraint activateConstraints:@[
                [mBanner.centerXAnchor constraintEqualToAnchor:guide.centerXAnchor],
                [mBanner.topAnchor constraintEqualToAnchor:guide.topAnchor]
            ]];
        }
        else
        {
            // this way will layout to the top of the screen but on the left side instead of centered.
            [NSLayoutConstraint activateConstraints:@[
                [guide.leftAnchor constraintEqualToAnchor:mBanner.leftAnchor],
                [guide.rightAnchor constraintEqualToAnchor:mBanner.rightAnchor],
                [guide.topAnchor constraintEqualToAnchor:mBanner.topAnchor]
            ]];
        }
    }
}

