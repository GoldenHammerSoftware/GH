// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHString/GHIdentifier.h"
#include <jni.h>

class GHPropertyContainer;
class GHHouseAd;
class GHJNIMgr;

class GHAndroidAdHandler : public GHController
{
public:
	GHAndroidAdHandler(GHJNIMgr& jniMgr,
                       jobject engineInterface,
                       GHPropertyContainer& props,
                       const GHIdentifier& displayingAdsProperty,
                       GHHouseAd* houseAd);
	virtual ~GHAndroidAdHandler(void);
	
	virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    
private:
    GHJNIMgr& mJNIMgr;
    jobject mEngineInterface;
    jmethodID mActivateAdsMethod;
    jmethodID mDeactivateAdsMethod;
    GHPropertyContainer& mProps;
    GHIdentifier mDisplayingAdsProperty;
    GHHouseAd* mHouseAd;
    bool mCurrentlyActive;
};
