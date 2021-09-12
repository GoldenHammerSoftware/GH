// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAdTransition.h"
#import <UIKit/UIKit.h>
#import <Chartboost/Chartboost.h>
#include "GHChartboost.h"
#include "GHiOSInitCallback.h"

class GHAdHandlingDelegate;
class GHChartboostTransition;

@interface GHChartboostDelegate : NSObject<CHBInterstitialDelegate>
{
    GHAdHandlingDelegate* mGHDelegate;
    void* mAdId;
    bool mIsShown;
    GHChartboostTransition* mParent;
};
@property(nonatomic, readwrite) void* mAdID;
@property(nonatomic, readwrite, assign) GHAdHandlingDelegate* mGHDelegate;
@property(nonatomic, readwrite) bool mIsShown;
@property(nonatomic, readwrite) GHChartboostTransition* mParent;

-(void) setGHDelegate:(GHAdHandlingDelegate*)ghDelegate andAdID:(void*)adID andParent:(GHChartboostTransition*)parent;
-(void) setShown:(bool)isShown;
@end

// show/hide a chartboost interstitial ad.
class GHChartboostTransition : public GHAdTransition
{
public:
    GHChartboostTransition(GHAdHandlingDelegate& ghAdDelegate, GHChartboost& chartboost, UIViewController* viewController);
    
    virtual void activate(void);
	virtual void deactivate(void);
    
    virtual bool hasAd(void) const { return mHasAd; }
    void setHasAd(bool val) { mHasAd = val; }
    
    void onInitSuccess(void);
    
private:
    class InitListener : public GHiOSInitCallback
    {
    public:
        InitListener(GHChartboostTransition& parent) : mParent(parent) { }
        virtual void onInitSuccess(void);
    private:
        GHChartboostTransition& mParent;
    } mInitListener;
    
private:
    UIViewController* mViewController;
    GHChartboostDelegate* mCBDelegate;
    CHBInterstitial* mInterstitial{nullptr};
    bool mHasAd;
};
