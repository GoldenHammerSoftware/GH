// Copyright 2010 Golden Hammer Software
#include "GHAndroidAdHandler.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/android/GHJNIMgr.h"

static bool sAdIsActive = false;

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_onAdActivation(JNIEnv* env, jobject obj)
{
	sAdIsActive = true;
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_onAdDeactivation(JNIEnv* env, jobject obj)
{
	sAdIsActive = false;
}

GHAndroidAdHandler::GHAndroidAdHandler(GHJNIMgr& jniMgr,
                                       jobject engineInterface,
                                       GHPropertyContainer& props,
                                       const GHIdentifier& displayingAdsProperty,
                                       GHHouseAd* houseAd)
: mJNIMgr(jniMgr)
, mEngineInterface(engineInterface)
, mProps(props)
, mHouseAd(houseAd)
, mDisplayingAdsProperty(displayingAdsProperty)
, mCurrentlyActive(false)
{
    jclass cls = mJNIMgr.getJNIEnv().GetObjectClass(mEngineInterface);
	if(cls == 0) {
		GHDebugMessage::outputString("GHAndroidAdHandler can't find class where activateBannerAd and deactivateBannerAd methods should exist\n.");
	}
	mActivateAdsMethod = mJNIMgr.getJNIEnv().GetMethodID(cls, "activateBannerAd", "()V");
	if(mActivateAdsMethod == 0) {
		GHDebugMessage::outputString("GHAndroidAdHandler can't find activateBannerAd method in Java.\n");
	}
    mDeactivateAdsMethod = mJNIMgr.getJNIEnv().GetMethodID(cls, "deactivateBannerAd", "()V");
	if(mDeactivateAdsMethod == 0) {
		GHDebugMessage::outputString("GHAndroidAdHandler can't find deactivateBannerAd method in Java.\n");
	}
}

GHAndroidAdHandler::~GHAndroidAdHandler(void)
{
	
}

void GHAndroidAdHandler::update(void)
{
    if ((mCurrentlyActive && !sAdIsActive)
        || (!mCurrentlyActive && sAdIsActive))
    {
        mProps.setProperty(mDisplayingAdsProperty, sAdIsActive ? 1 : 0);
        mCurrentlyActive = sAdIsActive;
    }
}

void GHAndroidAdHandler::onActivate(void)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mEngineInterface, mActivateAdsMethod);
}

void GHAndroidAdHandler::onDeactivate(void)
{
	// we are deactivating the ad on exit, so make sure the game knows
	mProps.setProperty(mDisplayingAdsProperty, 0);
	mCurrentlyActive = false;

	mJNIMgr.getJNIEnv().CallVoidMethod(mEngineInterface, mDeactivateAdsMethod);
}

