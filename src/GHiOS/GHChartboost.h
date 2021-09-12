#pragma once

#include <vector>

class GHiOSInitCallback;

class GHChartboost
{
public:
    GHChartboost(NSString* chartboostAppId, NSString* chartboostAppSignature);
    
    void initialize(GHiOSInitCallback* listener);
    
    bool isInitialized(void) const { return mInitialized; }
    
private:
    std::vector<GHiOSInitCallback*> mPendingListeners;
    NSString* mChartboostAppId;
    NSString* mChartboostAppSignature;
    bool mInitializationPending{false};
    bool mInitialized{false};
    bool mInitializationFailed{false};
};

