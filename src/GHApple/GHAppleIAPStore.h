// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHIAPStore.h"
#import <StoreKit/StoreKit.h>
#import "GHStoreKitDelegate.h"
#include <map>
#include <list>

class GHEventMgr;
class GHMessageQueue;
class GHAppleIAPMessageHandler;

// The Apple iOS StoreKit implementation of the in-app purchase store
class GHAppleIAPStore : public GHIAPStore
{
public:  
    GHAppleIAPStore(GHEventMgr& gameThreadEventMgr,
                  GHMessageQueue& gameThreadMessageQueue);
    ~GHAppleIAPStore(void);
    
    virtual void registerCallback(GHIAPCallback* callback);
    virtual void unregisterCallback(GHIAPCallback* callback);
    virtual void getCallbacks(std::vector<GHIAPCallback*>& ret) const;
    virtual void purchaseItem(int itemID);
    virtual void checkAllPurchases(void);
    
    void addItemID(int itemID, NSString* storeKitID);
    
    void handleTransactionComplete(SKPaymentTransaction* transaction, bool success);
    void handleTransactionComplete(NSString* transaction, bool success);
    void handleProductsFetched(NSArray* products);
    void handleNetworkError(NSError* error);
    void restorePurchases(void);
    virtual void displayIAPRestoreFailDialog(void) = 0;
    
protected:
    virtual void recordTransaction(SKPaymentTransaction* transaction) = 0;
    virtual void displayNetworkErrorDialog(NSError* error) = 0;
    
private:
    bool informGame(int itemID, bool success);
    bool canMakePurchases(void) const;
    void finishStoreKitProcessing(SKPaymentTransaction* transaction, bool wasSuccessful);
    
private:
    GHEventMgr& mGameThreadEventMgr;
    GHMessageQueue& mGameThreadMessageQueue;
    // A delegate to handle callbacks from StoreKit
    GHStoreKitDelegate* mStoreKitDelegate;
    GHAppleIAPMessageHandler* mMessageHandler;

    //A list of GH identifiers for potential purchases to identifiers
    // that we use to communicate with StoreKit
    struct Product
    {
        NSString* mStoreKitID;
        //SKProduct* mProduct;
        Product(void) : mStoreKitID(0)
        //, mProduct(0) 
        { }
    };
    typedef std::map<int, Product> ProductList;
    ProductList mProductList;
    
    //In the case that we receive a trasaction completed from StoreKit
    // before a callback is registered, we save it so that we can inform
    // the game when the callback is registered.
    struct Transaction
    {
        int mGHID;
        bool mWasSuccessful;
        Transaction(int ghID, bool wasSuccessful)
        : mGHID(ghID), mWasSuccessful(wasSuccessful)
        { }
    };
    typedef std::list<Transaction> TransactionList;
    TransactionList mUninformedTransactions;
};
