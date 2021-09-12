// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHIAPStore.h"
#include <list>
#include <jni.h>
#include "GHUtils/GHMessageHandler.h"

class GHEventMgr;
class GHMessageQueue;
class GHJNIMgr;

class GHAndroidIAPStore : public GHIAPStore
{
public:  
    GHAndroidIAPStore(GHJNIMgr& jniMgr, jobject engineInterface,
                      GHEventMgr& gameThreadEventMgr,
                      GHMessageQueue& gameThreadMessageQueue);
    ~GHAndroidIAPStore(void);

    virtual void registerCallback(GHIAPCallback* callback);
    virtual void unregisterCallback(GHIAPCallback* callback);
	virtual void getCallbacks(std::vector<GHIAPCallback*>& ret) const;
    virtual void purchaseItem(int itemID);
	virtual void checkAllPurchases(void);
    
    void onIAPPurchase(int isSuccess, int id);
    void onIAPPending(int id);

private:
    class CallbackCaller : public GHMessageHandler
    {
    public:
        CallbackCaller(GHEventMgr& eventMgr);
        ~CallbackCaller(void);
        void registerCallback(GHIAPCallback* callback);
        void unregisterCallback(GHIAPCallback* callback);
		void getCallbacks(std::vector<GHIAPCallback*>& ret) const;
        
        virtual void handleMessage(const GHMessage& message);
        
    private:
        GHEventMgr& mEventMgr;
        std::list<GHIAPCallback*> mCallbacks;
    };
    CallbackCaller mCallbackCaller;
    
private:
	GHJNIMgr& mJNIMgr;
    jobject mEngineInterface;
    jmethodID mPurchaseIAPMethod;
    jmethodID mCheckAllPurchasesMethod;
    
    GHMessageQueue& mGameThreadMessageQueue;
};
