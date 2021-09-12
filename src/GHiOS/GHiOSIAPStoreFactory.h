// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHAppleIAPStoreFactory.h"

//Creates the Apple iOS StoreKit implementation of the in-app purchase store
class GHiOSIAPStoreFactory : public GHAppleIAPStoreFactory
{
public:   
    GHiOSIAPStoreFactory(GHEventMgr& gameThreadEventMgr, GHMessageQueue& gameThreadMessageQueue);
    virtual GHIAPStore* createIAPStore(const char* configFilename,
                                       const GHXMLSerializer& xmlSerializer) const;
};
