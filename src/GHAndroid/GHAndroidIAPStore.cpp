// Copyright 2010 Golden Hammer Software
#include "GHAndroidIAPStore.h"
#include "GHIAPCallback.h"
#include "GHAndroidIdentifiers.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/android/GHJNIMgr.h"

static GHAndroidIAPStore* sGlobalIAPStore = 0;

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_onIAPPurchase(JNIEnv* env, jobject thiz, jint isSuccess, jint id)
{
    if (sGlobalIAPStore)
    {
        sGlobalIAPStore->onIAPPurchase(isSuccess, id);
    }
}

GHAndroidIAPStore::GHAndroidIAPStore(GHJNIMgr& jniMgr, jobject engineInterface,
                                     GHEventMgr& gameThreadEventMgr,
                                     GHMessageQueue& gameThreadMessageQueue)
: mJNIMgr(jniMgr)
, mEngineInterface(engineInterface)
, mCallbackCaller(gameThreadEventMgr)
, mGameThreadMessageQueue(gameThreadMessageQueue)
{
    sGlobalIAPStore = this;
    
    jclass cls = mJNIMgr.getJNIEnv().GetObjectClass(mEngineInterface);
	if(cls == 0) {
		GHDebugMessage::outputString("GHAndroidIAPStore can't find class where purchaseIAP and checkAllIAPs methods should exist\n.");
	}
	mPurchaseIAPMethod = mJNIMgr.getJNIEnv().GetMethodID(cls, "purchaseIAP", "(I)V");
	if(mPurchaseIAPMethod == 0) {
		GHDebugMessage::outputString("GHAndroidIAPStore can't find purchaseIAP method in Java.\n");
	}
    mCheckAllPurchasesMethod = mJNIMgr.getJNIEnv().GetMethodID(cls, "checkAllIAPs", "()V");
	if(mCheckAllPurchasesMethod == 0) {
		GHDebugMessage::outputString("GHAndroidIAPStore can't find checkAllIAPs method in Java.\n");
	}
}

GHAndroidIAPStore::~GHAndroidIAPStore(void)
{
    sGlobalIAPStore = 0;
}

void GHAndroidIAPStore::registerCallback(GHIAPCallback* callback)
{
    mCallbackCaller.registerCallback(callback);
}

void GHAndroidIAPStore::unregisterCallback(GHIAPCallback* callback)
{
    mCallbackCaller.unregisterCallback(callback);
}

void GHAndroidIAPStore::getCallbacks(std::vector<GHIAPCallback*>& ret) const
{
	mCallbackCaller.getCallbacks(ret);
}

void GHAndroidIAPStore::purchaseItem(int itemID)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mEngineInterface, mPurchaseIAPMethod, itemID);
}

void GHAndroidIAPStore::checkAllPurchases(void)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mEngineInterface, mCheckAllPurchasesMethod);
}

void GHAndroidIAPStore::onIAPPurchase(int isSuccess, int id)
{
    // send a message to our game thread handler
    GHMessage message(GHAndroidIdentifiers::IAPINFORMGAME);
    message.getPayload().setProperty(GHAndroidIdentifiers::IAPITEMID, id);
    message.getPayload().setProperty(GHAndroidIdentifiers::IAPSUCCESS, isSuccess);
    mGameThreadMessageQueue.handleMessage(message);
}

GHAndroidIAPStore::CallbackCaller::CallbackCaller(GHEventMgr& eventMgr)
: mEventMgr(eventMgr)
{
    mEventMgr.registerListener(GHAndroidIdentifiers::IAPINFORMGAME, *this);
}

GHAndroidIAPStore::CallbackCaller::~CallbackCaller(void)
{
    mEventMgr.unregisterListener(GHAndroidIdentifiers::IAPINFORMGAME, *this);
}

void GHAndroidIAPStore::CallbackCaller::registerCallback(GHIAPCallback* callback)
{
    mCallbacks.push_back(callback);
}

void GHAndroidIAPStore::CallbackCaller::unregisterCallback(GHIAPCallback* callback)
{
    std::list<GHIAPCallback*>::iterator iter = std::find(mCallbacks.begin(), mCallbacks.end(), callback);
    if (iter != mCallbacks.end())
    {
        mCallbacks.erase(iter);
    }
}

void GHAndroidIAPStore::CallbackCaller::getCallbacks(std::vector<GHIAPCallback*>& ret) const
{
	ret.clear();
	for (auto iter = mCallbacks.begin(); iter != mCallbacks.end(); iter++)
	{
		ret.push_back(*iter);
	}
}

void GHAndroidIAPStore::CallbackCaller::handleMessage(const GHMessage& message)
{
    int successVal = message.getPayload().getProperty(GHAndroidIdentifiers::IAPSUCCESS);
    int id = message.getPayload().getProperty(GHAndroidIdentifiers::IAPITEMID);
    std::list<GHIAPCallback*>::iterator iter = mCallbacks.begin(), iterEnd = mCallbacks.end();
    for (; iter != iterEnd; ++iter)
    {
        (*iter)->transactionComplete(successVal, id);
    }
}
