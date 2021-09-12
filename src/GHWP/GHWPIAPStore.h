// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHIAPStore.h"
#include <map>
#include <vector>
#include "GHString.h"
#include "GHWPEventInterface.h"
#include "GHMessageHandler.h"

class GHEventMgr;

class GHWPIAPStore : public GHIAPStore
{
public:
	GHWPIAPStore(PhoneDirect3DXamlAppComponent::GHWPEventInterface^ eventInterface,
		GHEventMgr& eventMgr);

	virtual void registerCallback(GHIAPCallback* callback);
    virtual void unregisterCallback(GHIAPCallback* callback);
    virtual void purchaseItem(int itemID);
	virtual void checkAllPurchases(void);

	void addItem(int itemID, const char* windowsStoreName, bool isTrialPurchase);

	void platformCallback(const char* windowsStoreName, bool success);

private:
	void buyTaskCallback(void);
	// return 0 if we don't have an itemID
	// otherwise make a string Windows can use.
	Platform::String^ getPlatformString(int itemID);
	void sendPurchaseCallback(bool bought, int id);

private:
	class MessageListener : public GHMessageHandler
	{
	public:
		MessageListener(GHWPIAPStore& parent) : mParent(parent) {}
		virtual void handleMessage(const GHMessage& message);

	private:
		GHWPIAPStore& mParent;
	};

private:
	MessageListener mMessageListener;
	PhoneDirect3DXamlAppComponent::GHWPEventInterface^ mEventInterface;
	GHEventMgr& mEventMgr;

	std::vector<GHIAPCallback*> mCallbacks;
	struct IAPEntry
	{
		IAPEntry(void) : mIsTrialPurchase(false) {}
		GHString mWindowsName;
		bool mIsTrialPurchase;
	};
	std::map<int, IAPEntry> mIAPs;
};
