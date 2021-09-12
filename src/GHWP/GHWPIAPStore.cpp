// Copyright 2010 Golden Hammer Software
#include "GHWPIAPStore.h"
#include "GHDebugMessage.h"
#include "GHIAPCallback.h"
#include <wrl.h>
#include <ppltasks.h>  
#include "GHWPIdentifiers.h"
#include "GHEventMgr.h"
#include "GHMessage.h"

GHWPIAPStore::GHWPIAPStore(PhoneDirect3DXamlAppComponent::GHWPEventInterface^ eventInterface,
						   GHEventMgr& eventMgr)
: mEventInterface(eventInterface)
, mEventMgr(eventMgr)
, mMessageListener(*this)
{
	mEventMgr.registerListener(GHWPIdentifiers::WP_IAPCALLBACK, mMessageListener);
}

void GHWPIAPStore::registerCallback(GHIAPCallback* callback)
{
	mCallbacks.push_back(callback);
}

void GHWPIAPStore::unregisterCallback(GHIAPCallback* callback)
{
	std::vector<GHIAPCallback*>::iterator iter;
	for (iter = mCallbacks.begin(); iter != mCallbacks.end(); ++iter)
	{
		if (*iter == callback) {
			mCallbacks.erase(iter);
			return;
		}
	}
}

void GHWPIAPStore::purchaseItem(int itemID)
{
	bool isTrialPurchase = false;
	std::map<int, IAPEntry>::iterator findIter = mIAPs.find(itemID);
	if (findIter != mIAPs.end()) {
		isTrialPurchase = findIter->second.mIsTrialPurchase;
	}

	Platform::String^ platformName = getPlatformString(itemID);
	if (!platformName && !isTrialPurchase) {
		sendPurchaseCallback(false, itemID);
		return;
	}

	mEventInterface->buyIAP(platformName, isTrialPurchase);
}

Platform::String^ GHWPIAPStore::getPlatformString(int itemID)
{
	std::map<int, IAPEntry>::iterator findIter = mIAPs.find(itemID);
	if (findIter == mIAPs.end()) {
		GHDebugMessage::outputString("Failed to find IAP by itemID");
		return nullptr;
	}

	const int BUF_SZ = 512;
	wchar_t wcharBuff[BUF_SZ];
	MultiByteToWideChar(CP_ACP, 0, findIter->second.mWindowsName.getChars(), -1, wcharBuff, BUF_SZ);
	Platform::String^ platformName = ref new Platform::String(wcharBuff);
	return platformName;
}

void GHWPIAPStore::addItem(int itemID, const char* windowsStoreName, bool isTrialPurchase)
{
	mIAPs[itemID].mWindowsName.setConstChars(windowsStoreName, GHString::CHT_COPY);
	mIAPs[itemID].mIsTrialPurchase = isTrialPurchase;
}

void GHWPIAPStore::sendPurchaseCallback(bool bought, int id)
{
	for (unsigned int i = 0; i < mCallbacks.size(); ++i) {
		mCallbacks[i]->transactionComplete(bought, id);
	}
}

void GHWPIAPStore::checkAllPurchases(void)
{
	std::map<int, IAPEntry>::iterator iapIter;
	for (iapIter = mIAPs.begin(); iapIter != mIAPs.end(); ++iapIter)
	{
		bool isTrialPurchase = iapIter->second.mIsTrialPurchase;
		Platform::String^ platformName = getPlatformString(iapIter->first);
		if (!platformName && !isTrialPurchase) {
			continue;
		}
		mEventInterface->confirmIAP(platformName, isTrialPurchase);
	}
}

void GHWPIAPStore::platformCallback(const char* name, bool success)
{
	std::map<int, IAPEntry>::iterator findIter;
	for (findIter = mIAPs.begin(); findIter != mIAPs.end(); ++findIter)
	{
		if (!strcmp(findIter->second.mWindowsName.getChars(), name))
		{
			sendPurchaseCallback(success, findIter->first);
			return;
		}
	}
	GHDebugMessage::outputString("Failed to find %s in the store", name);
	sendPurchaseCallback(false, 0);
}

void GHWPIAPStore::MessageListener::handleMessage(const GHMessage& message)
{
	bool purchased = message.getPayload().getProperty(GHWPIdentifiers::WP_IAPRESULT);
	const char* name = message.getPayload().getProperty(GHWPIdentifiers::WP_IAPNAME);
	mParent.platformCallback(name, purchased);
}
