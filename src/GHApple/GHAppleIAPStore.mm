// Copyright 2010 Golden Hammer Software
#include "GHAppleIAPStore.h"
#import <StoreKit/StoreKit.h>
#import "GHStoreKitDelegate.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHAppleIAPMessageHandler.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHUtils/GHMessage.h"
#include "GHAppleIdentifiers.h"

GHAppleIAPStore::GHAppleIAPStore(GHEventMgr& gameThreadEventMgr,
                                GHMessageQueue& gameThreadMessageQueue)
: mGameThreadEventMgr(gameThreadEventMgr)
, mGameThreadMessageQueue(gameThreadMessageQueue)
{
    mMessageHandler = new GHAppleIAPMessageHandler(*this);
    mGameThreadEventMgr.registerListener(GHAppleIdentifiers::IAPINFORMGAME, *mMessageHandler);
    mGameThreadEventMgr.registerListener(GHAppleIdentifiers::IAPRESTORE, *mMessageHandler);
    
    mStoreKitDelegate = [[GHStoreKitDelegate alloc] init];
    [mStoreKitDelegate setGHStore:this];
    
    [[SKPaymentQueue defaultQueue] addTransactionObserver:mStoreKitDelegate];
}

GHAppleIAPStore::~GHAppleIAPStore(void)
{    
    mProductList.clear();
    
    mGameThreadEventMgr.unregisterListener(GHAppleIdentifiers::IAPRESTORE, *mMessageHandler);
    mGameThreadEventMgr.unregisterListener(GHAppleIdentifiers::IAPINFORMGAME, *mMessageHandler);
    delete mMessageHandler;
}

void GHAppleIAPStore::registerCallback(GHIAPCallback* callback)
{
    mMessageHandler->registerCallback(callback);
    
    TransactionList::iterator iter = mUninformedTransactions.begin(), iterEnd = mUninformedTransactions.end();
    for(; iter != iterEnd; ++iter)
    {
        Transaction& transaction = *iter;
        informGame(transaction.mGHID, transaction.mWasSuccessful);
    }
    mUninformedTransactions.clear();
}

void GHAppleIAPStore::unregisterCallback(GHIAPCallback* callback)
{
    mMessageHandler->unregisterCallback(callback);
}

void GHAppleIAPStore::getCallbacks(std::vector<GHIAPCallback*>& ret) const
{
    mMessageHandler->getCallbacks(ret);
}

void GHAppleIAPStore::purchaseItem(int itemID)
{
    ProductList::iterator itemToPurchase = mProductList.find(itemID);
    if (itemToPurchase == mProductList.end())
    {
        GHDebugMessage::outputString("The game has requested that we purchase an item with a GH id (%d) that does not have a corresponding StoreKit id.", itemID);
        informGame(false, itemID);
        return;
    }
    
    NSSet *productIdentifiers = [NSSet setWithObject:itemToPurchase->second.mStoreKitID];
    SKProductsRequest* productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];
    productsRequest.delegate = mStoreKitDelegate;
    [productsRequest start];
}

void GHAppleIAPStore::checkAllPurchases(void)
{
    // in spirit, "restorePurchases" provides this functionality.
    // We use that instead because checkAllPurchases is called on launch,
    // which doesn't work so well in StoreKit
    
    // 2020 update: It seems the reason we removed this is it pops up a login dialog asking for the password.  SKReceipt might be the replacement we're supposed to use that wouldn't do this.  At bowling 2 release we just included a restore iap button to the options.
}

void GHAppleIAPStore::restorePurchases(void)
{
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

void GHAppleIAPStore::addItemID(int itemID, NSString* storeKitID)
{
    mProductList[itemID].mStoreKitID = storeKitID;
}

bool GHAppleIAPStore::canMakePurchases(void) const
{
    return [SKPaymentQueue canMakePayments];
}

void GHAppleIAPStore::handleTransactionComplete(SKPaymentTransaction* transaction, bool success)
{  
    if (success)
    {
        recordTransaction(transaction);
    }
    
    handleTransactionComplete(transaction.payment.productIdentifier, success);
    
    finishStoreKitProcessing(transaction, success);
}

void GHAppleIAPStore::handleTransactionComplete(NSString* transaction, bool success)
{
    ProductList::iterator iter = mProductList.begin(), iterEnd = mProductList.end();
    for (; iter != iterEnd; ++iter)
    {
        if ([iter->second.mStoreKitID isEqualToString:transaction])
        {
            if (!informGame(iter->first, success))
            {
                mUninformedTransactions.push_back(Transaction(iter->first, success));
            }
            return;
        }
    }
}

void GHAppleIAPStore::finishStoreKitProcessing(SKPaymentTransaction *transaction, bool wasSuccessful)
{
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    
    NSString* notification = wasSuccessful ? @"GHStoreKitDelegateTransactionSuccessful" : @"GHStoreKitDelegateTransactionFailed";
    [[NSNotificationCenter defaultCenter] postNotificationName:notification object:mStoreKitDelegate];
    
}

bool GHAppleIAPStore::informGame(int itemId, bool success)
{
    if (!mMessageHandler->hasCallbacks())
    {
        return false;
    }
    
    GHMessage message(GHAppleIdentifiers::IAPINFORMGAME);
    message.getPayload().setProperty(GHAppleIdentifiers::IAPITEMID, itemId);
    message.getPayload().setProperty(GHAppleIdentifiers::IAPSUCCESS, success);
    mGameThreadMessageQueue.handleMessage(message);
    return true;
}

void GHAppleIAPStore::handleNetworkError(NSError* error)
{
    displayNetworkErrorDialog(error);
    
    informGame(-1, false);
}
