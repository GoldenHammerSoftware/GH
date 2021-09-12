// Copyright 2010 Golden Hammer Software
#include "GHiOSIAPStore.h"

GHiOSIAPStore::GHiOSIAPStore(GHEventMgr& gameThreadEventMgr,
                             GHMessageQueue& gameThreadMessageQueue)
: GHAppleIAPStore(gameThreadEventMgr, gameThreadMessageQueue)
{
    
}

void GHiOSIAPStore::recordTransaction(SKPaymentTransaction* transaction)
{
    [[NSUserDefaults standardUserDefaults] setValue:transaction.transactionReceipt forKey:transaction.transactionIdentifier ];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

void GHiOSIAPStore::displayNetworkErrorDialog(NSError* error)
{
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"No network connection."
                                                    message:@"You must be connected to the internet in order to purchase pins."
                                                   delegate:nil
                                          cancelButtonTitle:@"OK"
                                          otherButtonTitles:nil];
    [alert show];
}

void GHiOSIAPStore::displayIAPRestoreFailDialog(void)
{
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Failed to restore IAP."
                                                    message:@"Check that you have purchased IAP or check your network connection."
                                                   delegate:nil
                                          cancelButtonTitle:@"OK"
                                          otherButtonTitles:nil];
    [alert show];
}
