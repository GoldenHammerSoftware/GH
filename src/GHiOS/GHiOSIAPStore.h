// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAppleIAPStore.h"

class GHiOSIAPStore : public GHAppleIAPStore
{
public:  
    GHiOSIAPStore(GHEventMgr& gameThreadEventMgr,
                  GHMessageQueue& gameThreadMessageQueue);
    
    virtual void displayIAPRestoreFailDialog(void);
protected:
    virtual void recordTransaction(SKPaymentTransaction* transaction);
    virtual void displayNetworkErrorDialog(NSError* error);
};
