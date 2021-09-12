#pragma once

#include "GHIAPStore.h"
#include <map>
#include <vector>
#include "GHString/GHString.h"

class GHMetroIAPStore : public GHIAPStore
{
public:
	GHMetroIAPStore(void);

	virtual void registerCallback(GHIAPCallback* callback);
    virtual void unregisterCallback(GHIAPCallback* callback);
	virtual void getCallbacks(std::vector<GHIAPCallback*>& ret) const;
    virtual void purchaseItem(int itemID);
	virtual void checkAllPurchases(void);

	void addItem(int itemID, const char* windowsStoreName, bool isTrialPurchase);

private:
	void buyTaskCallback(void);
	// return 0 if we don't have an itemID
	// otherwise make a string Windows can use.
	Platform::String^ getPlatformString(int itemID);
	void sendPurchaseCallback(bool bought, int id);

private:
	std::vector<GHIAPCallback*> mCallbacks;
	struct IAPEntry
	{
		IAPEntry(void) : mIsTrialPurchase(false) {}
		GHString mWindowsName;
		bool mIsTrialPurchase;
	};
	std::map<int, IAPEntry> mIAPs;

	// used for callbacks on purchase item from windows async tasks
	int mCurrentPurchasingItemID;
};
