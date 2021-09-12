#include "GHMetroIAPStore.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHIAPCallback.h"
#include <wrl.h>
#include <ppltasks.h>  

using namespace concurrency; 
using namespace Windows::ApplicationModel; 
using namespace Windows::ApplicationModel::Store; 
using namespace Windows::Foundation; 
using namespace Windows::Storage; 

//#define TESTING_IAP 1
#ifdef TESTING_IAP
	#define WIN8_IAP_APP CurrentAppSimulator
#else
	#define WIN8_IAP_APP CurrentApp
#endif

GHMetroIAPStore::GHMetroIAPStore(void)
: mCurrentPurchasingItemID(-1)
{
	auto licenseInformation = WIN8_IAP_APP::LicenseInformation; 
	if (licenseInformation->IsTrial) {
		GHDebugMessage::outputString("Trial app!");
	}

#ifdef TESTING_IAP
	// this is a hack just to make IAP work while debugging.
	// can't buy IAP as a trial
	WIN8_IAP_APP::RequestAppPurchaseAsync(false);
#endif
}

void GHMetroIAPStore::registerCallback(GHIAPCallback* callback)
{
	mCallbacks.push_back(callback);
}

void GHMetroIAPStore::unregisterCallback(GHIAPCallback* callback)
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

void GHMetroIAPStore::getCallbacks(std::vector<GHIAPCallback*>& ret) const
{
	std::vector<GHIAPCallback*>::const_iterator iter;
	for (iter = mCallbacks.begin(); iter != mCallbacks.end(); ++iter)
	{
		ret.push_back(*iter);
	}
}

void GHMetroIAPStore::purchaseItem(int itemID)
{
	bool isTrialPurchase = false;
	std::map<int, IAPEntry>::iterator findIter = mIAPs.find(itemID);
	if (findIter != mIAPs.end()) {
		isTrialPurchase = findIter->second.mIsTrialPurchase;
	}

	Platform::String^ platformName = getPlatformString(itemID);
	if (!platformName && !isTrialPurchase) {
		buyTaskCallback();
		return;
	}

	// mark that this is the item we are purchasing so we know what to do on completion callback.
	if (mCurrentPurchasingItemID != -1) {
		GHDebugMessage::outputString("Ignoring second call to purchase while a first is still active");
	}
	mCurrentPurchasingItemID = itemID;

	auto licenseInformation = WIN8_IAP_APP::LicenseInformation; 
	if (!isTrialPurchase)
	{
		if (!licenseInformation->ProductLicenses->Lookup(platformName)->IsActive) 
		{ 
			try {
				// attempting to buy
				create_task(WIN8_IAP_APP::RequestProductPurchaseAsync(platformName, false)).then([this](
					task<Platform::String^> currentTask) 
				{ 
					try {
						currentTask.get(); 
						buyTaskCallback();
					}
					catch(Platform::Exception^ exception) 
					{ 
						GHDebugMessage::outputString("Some sort of IAP exception");
						buyTaskCallback();
					} 
				}); 
			}
			catch(Platform::Exception^ exception)
			{
				GHDebugMessage::outputString("Exception on RequestProductPurchaseAsync");
				buyTaskCallback();
			}
		} 
		else 
		{ 
			GHDebugMessage::outputString("Already purchased");
			buyTaskCallback();
		}
	}
	else 
	{
		if (!licenseInformation->IsTrial)
		{
			GHDebugMessage::outputString("Already have the full version");
			buyTaskCallback();
		}
		else
		{
			create_task(WIN8_IAP_APP::RequestAppPurchaseAsync(false)).then([this](task<Platform::String^> currentTask) 
			{ 
				try {
					currentTask.get(); 
					buyTaskCallback();
				}
				catch(Platform::Exception^ exception) 
				{ 
					GHDebugMessage::outputString("Some sort of IAP exception");
					buyTaskCallback();
				} 
			}); 
		}
	}
}

Platform::String^ GHMetroIAPStore::getPlatformString(int itemID)
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

void GHMetroIAPStore::buyTaskCallback(void)
{
	bool isTrialPurchase = false;
	std::map<int, IAPEntry>::iterator findIter = mIAPs.find(mCurrentPurchasingItemID);
	if (findIter != mIAPs.end()) {
		isTrialPurchase = findIter->second.mIsTrialPurchase;
	}

	Platform::String^ platformName = getPlatformString(mCurrentPurchasingItemID);
	if (!platformName && !isTrialPurchase) {
		sendPurchaseCallback(false, mCurrentPurchasingItemID);
		mCurrentPurchasingItemID = -1;
		return;
	}

	auto licenseInformation = WIN8_IAP_APP::LicenseInformation; 
	if (!isTrialPurchase)
	{
		auto licenseLookup = licenseInformation->ProductLicenses->Lookup(platformName);
		if (licenseLookup->IsActive) 
		{ 
			GHDebugMessage::outputString("Purchase successful");
			sendPurchaseCallback(true, mCurrentPurchasingItemID);
		} 
		else 
		{ 
			GHDebugMessage::outputString("Some sort of IAP error");
			sendPurchaseCallback(false, mCurrentPurchasingItemID);
		} 
	}
	else
	{
		if (!licenseInformation->IsTrial)
		{
			GHDebugMessage::outputString("Full version purchased.");
			sendPurchaseCallback(true, mCurrentPurchasingItemID);
		}
		else
		{
			sendPurchaseCallback(false, mCurrentPurchasingItemID);
		}
	}
	mCurrentPurchasingItemID = -1;
}

void GHMetroIAPStore::addItem(int itemID, const char* windowsStoreName, bool isTrialPurchase)
{
	mIAPs[itemID].mWindowsName.setConstChars(windowsStoreName, GHString::CHT_COPY);
	mIAPs[itemID].mIsTrialPurchase = isTrialPurchase;
}

void GHMetroIAPStore::sendPurchaseCallback(bool bought, int id)
{
	for (unsigned int i = 0; i < mCallbacks.size(); ++i) {
		mCallbacks[i]->transactionComplete(bought, id);
	}
}

void GHMetroIAPStore::checkAllPurchases(void)
{
	auto licenseInformation = WIN8_IAP_APP::LicenseInformation;

	std::map<int, IAPEntry>::iterator iapIter;
	for (iapIter = mIAPs.begin(); iapIter != mIAPs.end(); ++iapIter)
	{
		bool isTrialPurchase = iapIter->second.mIsTrialPurchase;
		Platform::String^ platformName = getPlatformString(iapIter->first);
		if (!platformName && !isTrialPurchase) {
			continue;
		}
		if (!isTrialPurchase)
		{
			auto licenseLookup = licenseInformation->ProductLicenses->Lookup(platformName);
			if (licenseLookup->IsActive) 
			{
				sendPurchaseCallback(true, iapIter->first);
			}
		}
		else
		{
			if (!licenseInformation->IsTrial)
			{
				sendPurchaseCallback(true, iapIter->first);
			}
		}
	}
}

