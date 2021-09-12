#include "GHAndroidInterstitialAd.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"
#include "GHPlatform/android/GHJNIMgr.h"

static GHMessageQueue* sGlobalMessageQueue = 0;

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_onInterstitialDismiss(JNIEnv* env, jobject obj)
{
    if (sGlobalMessageQueue)
    {
        sGlobalMessageQueue->handleMessage(GHMessage(GHMessageTypes::INTERSTITIALADFINISHED));
    }
}

GHAndroidInterstitialAd::GHAndroidInterstitialAd(GHJNIMgr& jniMgr,
                                                 jobject javaObj,
                                                 GHMessageQueue& messageQueue)
: mJNIMgr(jniMgr)
, mJavaObj(javaObj)
, mMessageQueue(messageQueue)
, mShowAdMethod(0)
, mHideAdMethod(0)
{
    sGlobalMessageQueue = &messageQueue;
    
    jclass cls = mJNIMgr.getJNIEnv().GetObjectClass(mJavaObj);
    if (cls == 0) {
        GHDebugMessage::outputString("GHAndroidInterstitialAd can't find java class where methods exist.");
    }
    
    mShowAdMethod = mJNIMgr.getJNIEnv().GetMethodID(cls, "showInterstitialAd", "()V");
    mHideAdMethod = mJNIMgr.getJNIEnv().GetMethodID(cls, "hideInterstitialAd", "()V");
    
    if (mShowAdMethod == 0) {
        GHDebugMessage::outputString("GHAndroidInterstitialAd: could not link showInterstitialAd method");
    }
}

void GHAndroidInterstitialAd::activate(void)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mJavaObj, mShowAdMethod);
}

void GHAndroidInterstitialAd::deactivate(void)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mJavaObj, mHideAdMethod);
}

bool GHAndroidInterstitialAd::hasAd(void) const
{
    //Ad provider switching is handled in Java
    return true;
}

