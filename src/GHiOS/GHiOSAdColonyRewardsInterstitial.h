// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAdTransition.h"
#include "GHString/GHIdentifier.h"
#import <AdColony/AdColony.h>
#include "GHiOSAdColonyInitializer.h"

class GHMessageQueue;
class GHPropertyContainer;
class GHAdHandlingDelegate;
class GHiOSAdColonyRewardsInterstitial;

@interface GHAdColonyInterstitialDelegate : NSObject<AdColonyInterstitialDelegate>
{
};
@property(nonatomic, readwrite) GHiOSAdColonyRewardsInterstitial* _Nullable mParent;

- (void)adColonyInterstitialDidLoad:(AdColonyInterstitial * _Nonnull)interstitial;
- (void)adColonyInterstitialDidFailToLoad:(AdColonyAdRequestError * _Nonnull)error;
- (void)adColonyInterstitialDidClose:(AdColonyInterstitial * _Nonnull)interstitial;
- (void)adColonyInterstitialExpired:(AdColonyInterstitial * _Nonnull)interstitial;

-(void) setGHParent:(GHiOSAdColonyRewardsInterstitial*_Nonnull)parent;
@end

class GHiOSAdColonyRewardsInterstitial : public GHAdTransition
{
public:
    GHiOSAdColonyRewardsInterstitial(GHMessageQueue& messageQueue,
                                     GHAdHandlingDelegate& adHandlingDelegate,
                                     GHPropertyContainer& props,
                                     const GHIdentifier& availabilityProp,
                                     GHiOSAdColonyInitializer& initializer,
                                     NSString* zoneID,
                                     UIViewController* viewController);
    ~GHiOSAdColonyRewardsInterstitial(void);
    
    virtual void activate(void);
    virtual void deactivate(void);
    virtual bool hasAd(void) const;
    
    void onAvailabilityChange(bool available);
    void onReward(bool success);
    
    void handleZoneReturned(AdColonyZone* zone);
    
    void handleAdChange(bool adIsReady, AdColonyInterstitial* ad, AdColonyAdRequestError* error);
    
private:
    void requestInterstitial(void);
    
    class InitListener : public GHAdColonyInitCallback
    {
    public:
        InitListener(GHiOSAdColonyRewardsInterstitial& parent) : mParent(parent) { }
        
        virtual void onZoneReturned(AdColonyZone* zone);
    private:
        GHiOSAdColonyRewardsInterstitial& mParent;
    } mInitListener;
    
private:
    GHMessageQueue& mMessageQueue;
    GHAdHandlingDelegate& mAdHandlingDelegate;
    GHPropertyContainer& mProps;
    GHiOSAdColonyInitializer& mInitializer;
    GHIdentifier mAvailabilityProp;
    UIViewController* mTopViewController;
    GHAdColonyInterstitialDelegate* mAdColonyDelegate;
    
    AdColonyInterstitial * _Nullable mAd;
    NSString* _Nullable mZoneID;
};


