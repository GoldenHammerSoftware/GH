// Copyright 2010 Golden Hammer Software
#import "GHStoreKitDelegate.h"
#include "GHAppleIAPStore.h"

@implementation GHStoreKitDelegate

-(void)setGHStore:(GHAppleIAPStore*)iapStore
{
    miAPStore = iapStore;
}

-(void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    for (SKProduct* product in response.products) 
    {
        NSLog(@"Purchasing product %@ with id %@ for %@", product.localizedTitle, product.productIdentifier, product.price);
        SKPayment* payment = [SKPayment paymentWithProduct:product];
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    }
    
    for (NSString* invalidID in response.invalidProductIdentifiers)
    {
        NSLog(@"Invalid Product Id (%@) requested to StoreKit. Transaction failed.", invalidID);
        miAPStore->handleTransactionComplete(invalidID, false);
    }
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error 
{
    NSLog(@"StoreKit request %@ failed. Error: %@", [request description], [error description]);
    miAPStore->handleNetworkError(error);
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction* transaction in transactions)
    {
        SKPaymentTransactionState state = [transaction transactionState];
        switch (state)
        {
            case SKPaymentTransactionStatePurchased:
                miAPStore->handleTransactionComplete(transaction, true);
                break;
            case SKPaymentTransactionStateFailed:
                miAPStore->handleTransactionComplete(transaction, false);
                break;
            case SKPaymentTransactionStateRestored:
                miAPStore->handleTransactionComplete(transaction.originalTransaction, true);
                break;
            case SKPaymentTransactionStatePurchasing:
                //[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
        }
    }
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
    for (SKPaymentTransaction* transaction in queue.transactions)
    {
        miAPStore->handleTransactionComplete(transaction, true);
    }
}

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
    miAPStore->displayIAPRestoreFailDialog();
}

@end
