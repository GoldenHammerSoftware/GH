// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAdHandlingDelegate.h"
#include <vector>
#include "GHString/GHIdentifier.h"

class GHiOSAdHandler;
class GHPropertyContainer;
class GHiOSAdWrapper;
class GHTimeVal;

class GHiOSAdStack : public GHAdHandlingDelegate
{
public:
    GHiOSAdStack(GHPropertyContainer& props,
                 const GHIdentifier& displayingAdsProperty);
    ~GHiOSAdStack(void);

	virtual void adIsActive(void* adID);
	virtual void adDeactivated(void* adID);
    
    void update(const GHTimeVal& timeVal);
    void initialize(void);
    void activate(void);
    void deactivate(void);
    
    void addWrapper(GHiOSAdWrapper* wrapper);
    void updateIsPortrait(bool isPortrait);
    
private:
    void deactivateLowerPriorityAds(int adID);
    
private:
    GHPropertyContainer& mProps;
    GHIdentifier mDisplayingAdsProperty;
    
    class Advertisement
	{
    public:
        Advertisement(GHiOSAdWrapper* adWrapper)
        : mWrapper(adWrapper), mHasAnAd(false), mWasActivated(false)
        { }
        void initialize(GHAdHandlingDelegate* adHandlingDelegate, int adID);
        void destruct(void);
        void setHasAd(bool hasAd) { mHasAnAd = hasAd; }
        void setActive(bool active);
        void update(const GHTimeVal& timeVal);
        
        bool hasAnAd(void) const { return mHasAnAd; }
        bool wasActivated(void) const { return mWasActivated; }
        
        GHiOSAdWrapper* getWrapper(void) const { return mWrapper; }
        
    private:
		GHiOSAdWrapper* mWrapper;
		bool mHasAnAd;
        bool mWasActivated;
	};
	std::vector<Advertisement> mAdList;
	int mCurrentlyVisibleAd;
    bool mIsActive{false};
};
