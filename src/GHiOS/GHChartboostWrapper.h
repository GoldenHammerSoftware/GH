#pragma once

#include "GHiOSAdWrapper.h"
#include "GHChartboost.h"
#import <Chartboost/Chartboost.h>
#include "GHiOSInitCallback.h"

class GHChartboostWrapper;
class GHMessageHandler;

@interface GHChartboostBannerDelegate : NSObject<CHBBannerDelegate>
{
    GHAdHandlingDelegate* mGHDelegate;
    void* mAdId;
    GHChartboostWrapper* mParent;
    GHMessageHandler* mMessageHandler;
    bool mHasEverCachedAnAd;
};
@property(nonatomic, readwrite) void* mAdID;
@property(nonatomic, readwrite, assign) GHAdHandlingDelegate* mGHDelegate;
@property(nonatomic, readwrite) GHChartboostWrapper* mParent;
@property(nonatomic) bool mHasEverCachedAnAd;

-(void) setGHDelegate:(GHAdHandlingDelegate*)ghDelegate andAdID:(void*)adID andParent:(GHChartboostWrapper*)parent;
-(void) setMessageHandler:(GHMessageHandler*)messageHandler;
@end

class GHChartboostWrapper : public GHiOSAdWrapper
{
public:
    GHChartboostWrapper(UIView* view, GHMessageHandler& eventMgr, GHChartboost& chartboost);
    
    virtual ~GHChartboostWrapper(void);
    virtual void initialize(GHAdHandlingDelegate* adHandlingDelegate, int adID);
    virtual void setOrientation(bool isPortrait);
    virtual void update(const GHTimeVal& timeVal);
    virtual void setActive(bool active);
    
    void onInitSuccess(void);
    
private:
    void positionBannerInSafeArea(void);
    
private:
    class InitListener : public GHiOSInitCallback
    {
    public:
        InitListener(GHChartboostWrapper& parent) : mParent(parent) {}
        virtual void onInitSuccess(void);
    private:
        GHChartboostWrapper& mParent;
    } mInitListener;
    
private:
    UIView* mSuperView{nullptr};
    GHMessageHandler& mEventMgr;
    GHChartboost& mChartboost;
    GHAdHandlingDelegate* mGHDelegate{nullptr};
    CHBBanner* mBanner{nullptr};
    GHChartboostBannerDelegate* mCBDelegate{nullptr};
    bool mIsActive{false};
    bool mForceRefresh{false};
};

