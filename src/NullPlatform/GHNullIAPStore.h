// Copyright Golden Hammer Software
#pragma once

#include "Game/GHIAPStore.h"
#include <vector>

class GHIAPCallback;

// A mock IAP store.  Always returns true or false for purchases based on construction args.
class GHNullIAPStore : public GHIAPStore
{
public:
	GHNullIAPStore(bool alwaysBuy);

	virtual void registerCallback(GHIAPCallback* callback);
    virtual void unregisterCallback(GHIAPCallback* callback);
    virtual void getCallbacks(std::vector<GHIAPCallback*>& ret) const;
    virtual void purchaseItem(int itemID);
	virtual void checkAllPurchases(void);

	void addItem(int itemID);

private:
	std::vector<GHIAPCallback*> mCallbacks;
	std::vector<int> mIAPs;

	// our value for whether we approve or deny all purchases.
	bool mPurchaseResult;
};
