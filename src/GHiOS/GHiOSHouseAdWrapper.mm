// Copyright 2010 Golden Hammer Software
#include "GHiOSHouseAdWrapper.h"
#include "GHHouseAd.h"
#include "GHAdHandlingDelegate.h"
#include "GHPlatform/GHTimeVal.h"

static constexpr float kLaunchTimeWait = 5.0f;

GHiOSHouseAdWrapper::GHiOSHouseAdWrapper(GHHouseAd* houseAd)
: mHouseAd(houseAd)
, mAdHandlingDelegate(0)
, mAdID(0)
{
    
}

GHiOSHouseAdWrapper::~GHiOSHouseAdWrapper(void)
{
    delete mHouseAd;
}

void GHiOSHouseAdWrapper::initialize(GHAdHandlingDelegate* adHandlingDelegate, int adID)
{
    mAdID = adID;
    mAdHandlingDelegate = adHandlingDelegate;
}

void GHiOSHouseAdWrapper::setOrientation(bool isPortrait)
{
    
}

void GHiOSHouseAdWrapper::update(const GHTimeVal& timeVal)
{
    if(timeVal.getTime() > kLaunchTimeWait)
    {
        mLaunchTimeWaitHappened = true;
        
        if(mIsActive)
        {
            mHouseAd->activate();
            mAdHandlingDelegate->adIsActive(this);
        }
    }
}

void GHiOSHouseAdWrapper::setActive(bool active)
{
    if (active)
    {
        if(mLaunchTimeWaitHappened)
        {
            mHouseAd->activate();
            mAdHandlingDelegate->adIsActive(this);
        }
        mIsActive = true;
    }
    else 
    {
        mHouseAd->deactivate();
        mAdHandlingDelegate->adDeactivated(this);
        mIsActive = false;
    }
}
