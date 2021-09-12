#include "GHiOSAdColonyBanner.h"
#include "GHAdHandlingDelegate.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHMessageTypes.h"

@implementation GHAdColonyViewDelegate
@synthesize mAdID;
@synthesize mGHDelegate;
@synthesize mParent;
@synthesize mHasEverCachedAnAd;

-(void) setGHDelegate:(GHAdHandlingDelegate*)ghDelegate andAdID:(void*)adID andParent:(GHiOSAdColonyBanner*)parent
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

-(void)adColonyAdViewDidLoad:(AdColonyAdView *)adView
{
    mHasEverCachedAnAd = true;
    mParent->handleBannerLoaded(adView);
    mGHDelegate->adIsActive(mAdID);
}

-(void)adColonyAdViewDidClose:(AdColonyAdView *)adView
{
    
}

-(void)adColonyAdViewWillOpen:(AdColonyAdView *)adView
{
    
}

-(void)adColonyAdViewDidFailToLoad:(AdColonyAdRequestError *)error
{
    NSLog(@"AdColony banner request failed with error %@, reason %@ and suggestion: %@", error.localizedDescription, error.localizedFailureReason, error.localizedRecoverySuggestion);
    if(!mHasEverCachedAnAd)
    {
        mGHDelegate->adDeactivated(mAdID);
    }
}

-(void)adColonyAdViewDidReceiveClick:(AdColonyAdView *)adView
{
    
}

-(void)adColonyAdViewWillLeaveApplication:(AdColonyAdView *)adView
{
    
}

@end

GHiOSAdColonyBanner::GHiOSAdColonyBanner(UIView* view, GHMessageHandler& eventMgr, GHiOSAdColonyInitializer& initializer, NSString* zoneId)
: mInitListener(*this)
, mSuperView(view)
, mEventMgr(eventMgr)
, mZoneId(zoneId)
, mInitializer(initializer)
{
    
}

GHiOSAdColonyBanner::~GHiOSAdColonyBanner(void)
{
    
}

void GHiOSAdColonyBanner::initialize(GHAdHandlingDelegate* adHandlingDelegate, int adID)
{
    mACDelegate = [GHAdColonyViewDelegate alloc];
    [mACDelegate setGHDelegate:adHandlingDelegate andAdID:this andParent:this];
    
    mGHDelegate = adHandlingDelegate;
    mInitializer.initialize(&mInitListener, mZoneId);
}

void GHiOSAdColonyBanner::setOrientation(bool isPortrait)
{
    if(mBanner && mIsActive)
    {
        positionBannerInSafeArea();
    }
}

void GHiOSAdColonyBanner::update(const GHTimeVal& timeVal)
{
    
}

void GHiOSAdColonyBanner::setActive(bool active)
{
    if (active && !mIsActive) {
        if(mBanner) {
            [mSuperView addSubview:mBanner];
            positionBannerInSafeArea();
        }
        mForceRefresh = true;
        mIsActive = true;
        if (mGHDelegate && mACDelegate.mHasEverCachedAnAd) { mGHDelegate->adIsActive(this); }
    }
    else if (!active && mIsActive) {
        if(mBanner) {
            [mBanner removeFromSuperview];
        }
        mForceRefresh = false;
        mIsActive = false;
        if (mGHDelegate) { mGHDelegate->adDeactivated(this); }
    }
}

void GHiOSAdColonyBanner::handleBannerLoaded(AdColonyAdView* banner)
{
    if(mBanner)
    {
        [mBanner destroy];
    }
    
    mBanner = banner;
    
    if(mIsActive)
    {
        [mSuperView addSubview:mBanner];
        positionBannerInSafeArea();
    }
}

void GHiOSAdColonyBanner::onInitSuccess(void)
{
    UIViewController* viewController = nil;
    [AdColony requestAdViewInZone:mZoneId withSize:kAdColonyAdSizeBanner viewController:viewController andDelegate:mACDelegate];
}

void GHiOSAdColonyBanner::InitListener::onZoneReturned(AdColonyZone* zone)
{
    mParent.onInitSuccess();
}

void GHiOSAdColonyBanner::positionBannerInSafeArea(void)
{
    CGFloat heightOffset = 0;
    if(@available(ios 11.0, *))
    {
        heightOffset = mSuperView.safeAreaLayoutGuide.layoutFrame.origin.y;
    }

    mBanner.frame = CGRectMake(0,heightOffset,mSuperView.frame.size.width, mBanner.frame.size.height);
}

