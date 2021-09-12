#include "GHiOSAdColonyInitializer.h"
#include "GHiOSInitCallback.h"

GHiOSAdColonyInitializer::GHiOSAdColonyInitializer(NSString* appId, NSArray<NSString*>* zoneIds)
: mAppId(appId)
, mZoneIds(zoneIds)
{
}

GHiOSAdColonyInitializer::~GHiOSAdColonyInitializer(void)
{
    //"release" unavailable in automatic reference counting mode
    //[mZoneIds release];
}
    
void GHiOSAdColonyInitializer::initialize(GHAdColonyInitCallback* callback, NSString* zoneId)
{
    if(mInitialized)
    {
        if(callback)
        {
            for(AdColonyZone* zone in mZones)
            {
                if([[zone identifier] isEqualToString:zoneId])
                {
                    callback->onZoneReturned(zone);
                    break;
                }
            }
        }
        return;
    }
    
    if(mInitializationFailed) {
        return;
    }
    
    if(callback) {
        mPendingCallbacks.push_back(Callback(callback, zoneId));
    }
    
    if(mInitializationPending){
        return;
    }
    
    mInitializationPending = true;
    
    [AdColony configureWithAppID:mAppId zoneIDs:mZoneIds options:nil completion:^(NSArray<AdColonyZone*> *zones)
    {
        mInitialized = true;
        mInitializationFailed = false;
        mInitializationPending = false;
        
        mZones = zones;
        
        for(AdColonyZone* zone in mZones)
        {
            for(Callback& cb : mPendingCallbacks)
            {
                if(cb.mCallback &&
                   [[zone identifier] isEqualToString:cb.mZoneId])
                {
                    cb.mCallback->onZoneReturned(zone);
                }
            }
        }
        
        mPendingCallbacks.clear();

    }];
}

