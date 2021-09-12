// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHiOSAdWrapper.h"
#import <UIKit/UIKit.h>
#import <GoogleMobileAds/GADAdSize.h>
#include <CoreGraphics/CGGeometry.h>
#include "GHAdMobDelegate.h"

class GHMessageHandler;

class GHAdMobWrapper : public GHiOSAdWrapper
{
public:
	GHAdMobWrapper(UIView* view, GHMessageHandler& eventMgr, NSString* publisherID);
	virtual ~GHAdMobWrapper(void);
	virtual void initialize(GHAdHandlingDelegate* adHandlingDelegate, int adID);
	virtual void setOrientation(bool isPortrait);
	virtual void update(const GHTimeVal& timeVal);
	virtual void setActive(bool active);
    
private:
    void createAdMobBanner(void);
    void calcSize(bool isPortrait, GADAdSize& size);
    void positionBannerViewInSafeArea(void);
	
private:
    UIView* mSuperView;
    GHMessageHandler& mMessageHandler;
	NSString* mPublisherID;
	GADBannerView* mAdMobView;
	GHAdMobDelegate* mAdMobDelegate;
    GHAdHandlingDelegate* mGHDelegate;
	UIViewController* mViewController;
	bool mForceRefresh;
	float mLastRefreshTime;
    bool mIsPortrait;
    bool mIsActive;
    bool mInitialized;
};
