// Copyright 2010 Golden Hammer Software
#include "GHCocoaIAPStore.h"

GHCocoaIAPStore::GHCocoaIAPStore(GHEventMgr& gameThreadEventMgr,
                                 GHMessageQueue& gameThreadMessageQueue)
: GHAppleIAPStore(gameThreadEventMgr, gameThreadMessageQueue)
{
    
}

void GHCocoaIAPStore::recordTransaction(SKPaymentTransaction* transaction)
{
    
}

void GHCocoaIAPStore::displayNetworkErrorDialog(NSError* error)
{
    [NSApp presentError:error];
}

void GHCocoaIAPStore::displayIAPRestoreFailDialog(void)
{
    dispatch_async(dispatch_get_main_queue(), ^
    {
        NSMutableDictionary* dictionary = [NSMutableDictionary dictionary];
        [dictionary setValue:@"Failed to restore IAP. Check that you have purchased IAP or check your network connection." forKey:NSLocalizedDescriptionKey];
        NSError* error = [NSError errorWithDomain:@"IAPFail" code:100 userInfo:dictionary];
        [NSApp presentError:error];
    });
}
