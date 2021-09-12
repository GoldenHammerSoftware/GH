// Copyright 2010 Golden Hammer Software
#pragma once

#import <StoreKit/StoreKit.h>

class GHAppleIAPStore;

@interface GHStoreKitDelegate : NSObject<SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
    GHAppleIAPStore* miAPStore;
}

-(void)setGHStore:(GHAppleIAPStore*)iapStore;

@end

