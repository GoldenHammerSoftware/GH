#pragma once

#include "GHiOSAdWrapper.h"
#import <AdColony/AdColony.h>
#include "GHiOSAdColonyInitializer.h"

class GHiOSAdColonyBanner;
class GHMessageHandler;

@interface GHAdColonyViewDelegate : NSObject<AdColonyAdViewDelegate>
{
    GHAdHandlingDelegate* mGHDelegate;
    void* mAdId;
    GHiOSAdColonyBanner* mParent;
    GHMessageHandler* mMessageHandler;
    bool mHasEverCachedAnAd;
};
@property(nonatomic, readwrite) void* mAdID;
@property(nonatomic, readwrite, assign) GHAdHandlingDelegate* mGHDelegate;
@property(nonatomic, readwrite) GHiOSAdColonyBanner* mParent;
@property(nonatomic) bool mHasEverCachedAnAd;
@property(nonatomic, weak) AdColonyAdView* mBanner;

-(void) setGHDelegate:(GHAdHandlingDelegate*)ghDelegate andAdID:(void*)adID andParent:(GHiOSAdColonyBanner*)parent;
-(void) setMessageHandler:(GHMessageHandler*)messageHandler;
@end

class GHiOSAdColonyBanner : public GHiOSAdWrapper
{
public:
    GHiOSAdColonyBanner(UIView* view, GHMessageHandler& eventMgr, GHiOSAdColonyInitializer& initializer, NSString* zoneId);
    
    virtual ~GHiOSAdColonyBanner(void);
    virtual void initialize(GHAdHandlingDelegate* adHandlingDelegate, int adID);
    virtual void setOrientation(bool isPortrait);
    virtual void update(const GHTimeVal& timeVal);
    virtual void setActive(bool active);
    
    void onInitSuccess(void);
    void handleBannerLoaded(AdColonyAdView* banner);
    
private:
    void positionBannerInSafeArea(void);
    
private:
    class InitListener : public GHAdColonyInitCallback
    {
    public:
        InitListener(GHiOSAdColonyBanner& parent) : mParent(parent) {}
        virtual void onZoneReturned(AdColonyZone* zone);
    private:
        GHiOSAdColonyBanner& mParent;
    } mInitListener;
    
private:
    UIView* mSuperView{nullptr};
    GHMessageHandler& mEventMgr;
    GHiOSAdColonyInitializer& mInitializer;
    NSString* mZoneId;
    GHAdHandlingDelegate* mGHDelegate{nullptr};
    GHAdColonyViewDelegate* mACDelegate{nullptr};
    AdColonyAdView* mBanner{nullptr};
    bool mIsActive{false};
    bool mForceRefresh{false};
};

