// Copyright 2010 Golden Hammer Software
#include "GHAndroidRewardsInterstitial.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/android/GHJNIMgr.h"

static GHMessageQueue* sGlobalMessageQueue = 0;
static GHAndroidRewardsInterstitial* sRewardsInterstitialHandler = 0;
static bool sIsAvailable = false;

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_onInterstitialRewardGranted(JNIEnv* env, jobject obj)
{
    if (sGlobalMessageQueue)
    {
		sGlobalMessageQueue->handleMessage(GHMessage(GHMessageTypes::INTERSTITIALREWARDGRANTED));
    }
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_onRewardInterstitialAvailabilityChange(JNIEnv* env, jobject obj, jboolean available)
{
	sIsAvailable = available;
	if (sRewardsInterstitialHandler) {
		sRewardsInterstitialHandler->onAvailabilityChange(available);
	}
}

GHAndroidRewardsInterstitial::GHAndroidRewardsInterstitial(GHJNIMgr& jniMgr,
                                                 jobject javaObj,
                                                 GHMessageQueue& messageQueue,
												 GHPropertyContainer& props,
												 const GHIdentifier& availabilityProp)
: mJNIMgr(jniMgr)
, mJavaObj(javaObj)
, mMessageQueue(messageQueue)
, mProps(props)
, mShowAdMethod(0)
, mHideAdMethod(0)
, mAvailabilityProp(availabilityProp)
{
    sGlobalMessageQueue = &messageQueue;
    
    jclass cls = mJNIMgr.getJNIEnv().GetObjectClass(mJavaObj);
    if (cls == 0) {
        GHDebugMessage::outputString("GHAndroidRewardsInterstitial can't find java class where methods exist.");
    }
    
    mShowAdMethod = mJNIMgr.getJNIEnv().GetMethodID(cls, "showRewardsInterstitial", "()V");
    mHideAdMethod = mJNIMgr.getJNIEnv().GetMethodID(cls, "hideRewardsInterstitial", "()V");
    
    if (mShowAdMethod == 0) {
        GHDebugMessage::outputString("GHAndroidRewardsInterstitial: could not link showRewardsInterstitial method");
    }
	sRewardsInterstitialHandler = this;
	onAvailabilityChange(sIsAvailable);
}

GHAndroidRewardsInterstitial::~GHAndroidRewardsInterstitial(void)
{
	sRewardsInterstitialHandler = 0;
}

void GHAndroidRewardsInterstitial::activate(void)
{
	GHDebugMessage::outputString("about to show rewards ad");
	mJNIMgr.getJNIEnv().CallVoidMethod(mJavaObj, mShowAdMethod);
	GHDebugMessage::outputString("showed rewards ad");
}

void GHAndroidRewardsInterstitial::deactivate(void)
{
	GHDebugMessage::outputString("about to hide rewards ad");
	mJNIMgr.getJNIEnv().CallVoidMethod(mJavaObj, mHideAdMethod);
	GHDebugMessage::outputString("rewards ad hidden");
}

bool GHAndroidRewardsInterstitial::hasAd(void) const
{
    //Ad provider switching is handled in Java
    return true;
}

void GHAndroidRewardsInterstitial::onAvailabilityChange(bool available)
{
	mProps.setProperty(mAvailabilityProp, available);
}
