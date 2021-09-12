// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAppleIAPStore.h"

class GHCocoaIAPStore : public GHAppleIAPStore
{
public:
    GHCocoaIAPStore(GHEventMgr& gameThreadEventMgr,
                    GHMessageQueue& gameThreadMessageQueue);
protected:
    virtual void recordTransaction(SKPaymentTransaction* transaction);
    virtual void displayNetworkErrorDialog(NSError* error);
    virtual void displayIAPRestoreFailDialog(void);
};
