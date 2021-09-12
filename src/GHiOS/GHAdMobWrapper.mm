// Copyright 2010 Golden Hammer Software
#include "GHAdMobWrapper.h"
#include "GHAdMobDelegate.h"
#include "GHPlatform/GHTimeVal.h"
#import <GoogleMobileAds/GADBannerView.h>
#include "GHAdHandlingDelegate.h"
#import <GoogleMobileAds/GoogleMobileAds.h>

// One of us thought the following statement was true at one point
//AdMob will refuse ads that are requested more frequently than every 12 seconds.
// DJW is trying to fix ad rotation by triggering a refresh and the example code does not have a minimum set.
const static float kMinRefreshPeriod = 0.f;

//We want to refresh about this often.
const static float kMaxRefreshPeriod = 45.f;

GHAdMobWrapper::GHAdMobWrapper(UIView* view, GHMessageHandler& messageHandler, NSString* publisherID)
: mPublisherID(publisherID)
, mAdMobDelegate(0)
, mGHDelegate(0)
, mSuperView(view)
, mMessageHandler(messageHandler)
, mForceRefresh(false)
, mLastRefreshTime(0)
, mAdMobView(0)
, mIsPortrait(true)
, mIsActive(false)
, mInitialized(false)
{
}

GHAdMobWrapper::~GHAdMobWrapper(void)
{
}

void GHAdMobWrapper::initialize(GHAdHandlingDelegate* adHandlingDelegate, int adID)
{
    [[GADMobileAds sharedInstance] startWithCompletionHandler:nil];
    
    GADMobileAds.sharedInstance.requestConfiguration.testDeviceIdentifiers =
        @[
            @"9197c14f02c61d046fd84f8e20e9d503",   //Dave's phone
            @"16570535d73cf2fc34ac1c23cab65740", //Dave's ipad
            @"17bd7c826098c2d5e439a6e7b00425c7" //Kevin's phone
        ];

    mGHDelegate = adHandlingDelegate;
    mViewController = [UIViewController alloc];
	
	mAdMobDelegate = [GHAdMobDelegate alloc];
	[mAdMobDelegate setAdHandlingDelegate:adHandlingDelegate withID:this];
	[mAdMobDelegate setMessageHandler:&mMessageHandler];
	[mAdMobDelegate setAdMobWrapper:this];

    createAdMobBanner();
    mInitialized = true;
}

void GHAdMobWrapper::createAdMobBanner(void)
{
    GADAdSize adSize;
    calcSize(mIsPortrait, adSize);
    mAdMobView = [[GADBannerView alloc] initWithAdSize:adSize];
    mAdMobView.adUnitID = mPublisherID;
    mAdMobView.rootViewController = mViewController;
    mAdMobView.translatesAutoresizingMaskIntoConstraints = false; // Need to set this to false to make constraints work.
    [mAdMobView setDelegate:mAdMobDelegate];
    [mViewController setView:mAdMobView];
}

void GHAdMobWrapper::calcSize(bool isPortrait, GADAdSize& size)
{
    int screenWidth = [mSuperView frame].size.width;
    int screenHeight = [mSuperView frame].size.height;
    // account for the crazy ass new iphones
    if (screenWidth > screenHeight*2)
    {
        screenWidth /= 4;
    }
    else if (screenWidth > screenHeight)
    {
        // landscape ads are huge, make them smaller.
        screenWidth /= 2;
    }
    size = GADCurrentOrientationAnchoredAdaptiveBannerAdSizeWithWidth(screenWidth);
}

void GHAdMobWrapper::setOrientation(bool isPortrait)
{
    if (isPortrait == mIsPortrait) {
        return;
    }
    mIsPortrait = isPortrait;
    if (mInitialized)
    {
        if (mAdMobView)
        {
            GADAdSize size;
            calcSize(mIsPortrait, size);
            [mAdMobView setAdSize:size];
        }
    }
}

void GHAdMobWrapper::update(const GHTimeVal& timeVal)
{	
	float refreshPeriod = mForceRefresh ? kMinRefreshPeriod : kMaxRefreshPeriod;
	if (timeVal.getTime() - mLastRefreshTime > refreshPeriod) {
        GADRequest* request = [GADRequest request];
		[mAdMobView loadRequest:request];
		mLastRefreshTime = timeVal.getTime();
		mForceRefresh = false;
	}
}

void GHAdMobWrapper::setActive(bool isActive)
{
	if (isActive && !mIsActive) {
		[mSuperView addSubview:mAdMobView];
        positionBannerViewInSafeArea();
		mForceRefresh = true;
        mIsActive = true;
        if (mGHDelegate && [mAdMobDelegate hasEverCachedAnAd]) { mGHDelegate->adIsActive(this); }
	}
	else if (!isActive && mIsActive) {
 		[mAdMobView removeFromSuperview];
        mForceRefresh = false;
        mIsActive = false;
        if (mGHDelegate) { mGHDelegate->adDeactivated(this); }
	}
}

void GHAdMobWrapper::positionBannerViewInSafeArea(void)
{
    if (@available(ios 11.0, *))
    {
        UILayoutGuide* guide = mSuperView.safeAreaLayoutGuide;
        [NSLayoutConstraint activateConstraints:@[
          [guide.leftAnchor constraintEqualToAnchor:mAdMobView.leftAnchor],
          [guide.rightAnchor constraintEqualToAnchor:mAdMobView.rightAnchor],
          [guide.topAnchor constraintEqualToAnchor:mAdMobView.topAnchor]
        ]];
    }
}
