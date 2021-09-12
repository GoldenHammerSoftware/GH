// Copyright Golden Hammer Software
#include "GHNullIAPStore.h"
#include "GHIAPCallback.h"
#include <stddef.h>
#include <algorithm>

GHNullIAPStore::GHNullIAPStore(bool alwaysBuy)
: mPurchaseResult(alwaysBuy)
{
}

void GHNullIAPStore::registerCallback(GHIAPCallback* callback)
{
	mCallbacks.push_back(callback);
}

void GHNullIAPStore::unregisterCallback(GHIAPCallback* callback)
{
	std::vector<GHIAPCallback*>::iterator iter = std::find(mCallbacks.begin(), mCallbacks.end(), callback);
	if (iter != mCallbacks.end())
	{
		mCallbacks.erase(iter);
	}
}

void GHNullIAPStore::getCallbacks(std::vector<GHIAPCallback*>& ret) const
{
    ret.clear();
    size_t numCallbacks = mCallbacks.size();
    for (size_t i = 0; i < numCallbacks; ++i)
    {
        ret.push_back(mCallbacks[i]);
    }
}

void GHNullIAPStore::purchaseItem(int itemID)
{
	size_t numCallbacks = mCallbacks.size();
	for (size_t i = 0; i < numCallbacks; ++i)
	{
		// call with false to use the trial version.
		mCallbacks[i]->transactionComplete((int)mPurchaseResult, itemID);
	}
}

void GHNullIAPStore::checkAllPurchases(void)
{
	size_t numItems = mIAPs.size();
	for (size_t i = 0; i < numItems; ++i)
	{
		int item = mIAPs[i];
		purchaseItem(item);
	}
}

void GHNullIAPStore::addItem(int itemID)
{
	mIAPs.push_back(itemID);
}
