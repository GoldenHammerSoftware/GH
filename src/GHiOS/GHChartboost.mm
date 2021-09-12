#include "GHChartboost.h"
#import <Chartboost/Chartboost.h>
#include "GHiOSInitCallback.h"

GHChartboost::GHChartboost(NSString* chartboostAppId, NSString* chartboostAppSignature)
: mChartboostAppId(chartboostAppId)
, mChartboostAppSignature(chartboostAppSignature)
{
    
}
    
void GHChartboost::initialize(GHiOSInitCallback* listener)
{
    if (mInitialized)
    {
        if (listener)
        {
            listener->onInitSuccess();
        }
        return;
    }
    
    if (mInitializationFailed) {
        return;
    }
    
    if (listener) {
        mPendingListeners.push_back(listener);
    }
    
    if (mInitializationPending){
        return;
    }
    
    mInitializationPending = true;
    [Chartboost startWithAppId:mChartboostAppId appSignature:mChartboostAppSignature completion:^(BOOL success) {
        mInitialized = success;
        mInitializationFailed = !mInitialized;
        mInitializationPending = false;
        
        if (mInitialized)
        {
            for (GHiOSInitCallback* l :mPendingListeners)
            {
                l->onInitSuccess();
            }
            mPendingListeners.clear();
        }
    }];

}

