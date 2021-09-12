// Copyright Golden Hammer Software
#pragma once

#include <vector>

class GHIAPCallback;

//An interface for dealing with a platform-specific store for
// buying in-app purchases
class GHIAPStore
{
public:
    virtual ~GHIAPStore(void) { }
    
    virtual void registerCallback(GHIAPCallback* callback) = 0;
    virtual void unregisterCallback(GHIAPCallback* callback) = 0;
    virtual void getCallbacks(std::vector<GHIAPCallback*>& ret) const = 0;
    virtual void purchaseItem(int itemID) = 0;
	virtual void checkAllPurchases(void) = 0;
};
