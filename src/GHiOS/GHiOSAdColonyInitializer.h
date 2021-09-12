#pragma once

#include <vector>
#import <AdColony/AdColony.h>

class GHAdColonyInitCallback
{
public:
    virtual ~GHAdColonyInitCallback(void) { }
    virtual void onZoneReturned(AdColonyZone* zone) = 0;
};

class GHiOSAdColonyInitializer
{
public:
    GHiOSAdColonyInitializer(NSString* appId, NSArray<NSString*>* zoneIds);
    ~GHiOSAdColonyInitializer(void);
    
    void initialize(GHAdColonyInitCallback* callback, NSString* zoneId);
    
    bool isInitialized(void) const { return mInitialized; }
    
private:
    struct Callback
    {
        GHAdColonyInitCallback* mCallback;
        NSString* mZoneId;
        Callback(GHAdColonyInitCallback* callback, NSString* zoneId)
        : mCallback(callback), mZoneId(zoneId)
        { }
    };
    std::vector<Callback> mPendingCallbacks;
    NSArray<AdColonyZone*>* mZones;
    NSString* mAppId;
    NSArray<NSString*>* mZoneIds;
    bool mInitializationPending{false};
    bool mInitialized{false};
    bool mInitializationFailed{false};
};

