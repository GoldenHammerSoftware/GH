// Copyright 2010 Golden Hammer Software
#include "GHiOSAdStack.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHiOSAdWrapper.h"

GHiOSAdStack::GHiOSAdStack(GHPropertyContainer& props,
                           const GHIdentifier& displayingAdsProperty)
: mProps(props)
, mDisplayingAdsProperty(displayingAdsProperty)
, mCurrentlyVisibleAd(-1)
{
    
}

GHiOSAdStack::~GHiOSAdStack(void)
{
    size_t numAds = mAdList.size();
	for (size_t i = 0; i < numAds; ++i) {
        mAdList[i].destruct();
	}
}

void GHiOSAdStack::adIsActive(void* adID)
{
    int adIdx = 0;
    for (int i = 0; i < mAdList.size(); ++i) {
        if (adID == mAdList[i].getWrapper()) {
            adIdx = i;
            break;
        }
    }
    
	mAdList[adIdx].setHasAd(true);
    
	if (adIdx < mCurrentlyVisibleAd || mCurrentlyVisibleAd < 0) {
		deactivateLowerPriorityAds(adIdx);
		mCurrentlyVisibleAd = adIdx;
	}
    
    bool higherPriorityAdExists = false;
    for (int i = 0; i < adIdx; ++i) {
        Advertisement& ad = mAdList[i];
        if (ad.hasAnAd()) {
            mCurrentlyVisibleAd = i;
            higherPriorityAdExists = true;
        }
    }
    
    if(!mIsActive)
    {
        return;
    }
    
    if (higherPriorityAdExists) {
        mAdList[adIdx].setActive(false);
    }
    else {
        mAdList[adIdx].setActive(true);
    }
	
	mProps.setProperty(mDisplayingAdsProperty, 1);
}

void GHiOSAdStack::adDeactivated(void* adID)
{
    int adIdx = 0;
    for (int i = 0; i < mAdList.size(); ++i) {
        if (adID == mAdList[i].getWrapper()) {
            adIdx = i;
            break;
        }
    }
    
	mAdList[adIdx].setHasAd(false);
	
	if (mIsActive &&
        adIdx == mCurrentlyVisibleAd)
    {
        mCurrentlyVisibleAd = (mCurrentlyVisibleAd + 1) % mAdList.size();
        
        Advertisement& ad = mAdList[mCurrentlyVisibleAd];
        ad.setActive(true);
	}
    
    bool anAdExists = false;
    if (mIsActive)
    {
        for (int i = 0; i < mAdList.size(); ++i)
        {
            Advertisement& ad = mAdList[i];
            if (ad.hasAnAd()) {
                ad.setActive(true);
                anAdExists = true;
                break;
            }
        }
    }
    
    
    if (!anAdExists) {
        mProps.setProperty(mDisplayingAdsProperty, 0);
    }
    else {
        // shouldn't be necessary.  should either already be showing or have it about to be set.
        mProps.setProperty(mDisplayingAdsProperty, 1);
    }
}

void GHiOSAdStack::initialize(void)
{
    mProps.setProperty(mDisplayingAdsProperty, 0);
    size_t numAds = mAdList.size();
    for (size_t i = 0;i < numAds; ++i) {
        mAdList[i].initialize(this, (int)i);
    }
}

void GHiOSAdStack::activate(void)
{
    mIsActive = true;
    size_t numAds = mAdList.size();
    for (size_t i = 0; i < numAds; ++i) {
        mAdList[i].setActive(true);
    }

    mCurrentlyVisibleAd = numAds ? 0 : -1;
    for (int i = 0; i < numAds; ++i) {
        if (mAdList[i].hasAnAd()) {
            mCurrentlyVisibleAd = i;
            break;
        }
    }
}

void GHiOSAdStack::deactivate(void)
{
    mIsActive = false;
    size_t numAds = mAdList.size();
    for (size_t i = 0;i < numAds; ++i) {
        Advertisement& ad = mAdList[i];
        ad.setActive(false);
    }    
    mCurrentlyVisibleAd = -1;
}

void GHiOSAdStack::updateIsPortrait(bool isPortrait)
{
    for (int i = 0; i < mAdList.size(); ++i)
    {
        mAdList[i].getWrapper()->setOrientation(isPortrait);
    }
}

void GHiOSAdStack::update(const GHTimeVal& timeVal)
{
    //Our currently visible ad could be our house ad, which
    // is always active, but we may not want to update the ads
    // in between the house ad and the currently active but unfilled ad
    
    for (int i = 0; i <= mCurrentlyVisibleAd; ++i) {
        Advertisement& ad = mAdList[i];
        if (ad.wasActivated()) {
            ad.update(timeVal);
        }
    }
}

void GHiOSAdStack::deactivateLowerPriorityAds(int adID)
{
	int numAds = mAdList.size();
	for (int i = adID + 1; i < numAds; ++i)
	{
		Advertisement& ad = mAdList[i];
		ad.setActive(false);
	}
}

void GHiOSAdStack::addWrapper(GHiOSAdWrapper* wrapper)
{
    Advertisement ad(wrapper);
	mAdList.push_back(ad);
}

void GHiOSAdStack::Advertisement::initialize(GHAdHandlingDelegate* adHandlingDelegate, int adID)
{
    mWrapper->initialize(adHandlingDelegate, adID);
}

void GHiOSAdStack::Advertisement::setActive(bool active)
{
    if ((active && !mWasActivated)
        || (!active && mWasActivated))
    {
        mWasActivated = active;
        mWrapper->setActive(active);
    }
}

void GHiOSAdStack::Advertisement::update(const GHTimeVal& timeVal)
{
    mWrapper->update(timeVal);
}

void GHiOSAdStack::Advertisement::destruct(void)
{
    delete mWrapper;
    mWrapper = 0;
}
