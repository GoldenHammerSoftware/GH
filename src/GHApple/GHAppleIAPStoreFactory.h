// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHIAPStoreFactory.h"

class GHXMLSerializer;
class GHEventMgr;
class GHMessageQueue;
class GHAppleIAPStore;

//Creates the Apple iOS StoreKit implementation of the in-app purchase store
class GHAppleIAPStoreFactory : public GHIAPStoreFactory
{
public:   
    GHAppleIAPStoreFactory(GHEventMgr& gameThreadEventMgr, GHMessageQueue& gameThreadMessageQueue);
    
    //To be implemented by derived class
    //virtual GHIAPStore* createIAPStore(const char* configFilename,
    //                                   const GHXMLSerializer& xmlSerializer) const;
    
protected:
    void fillIAPStore(GHAppleIAPStore& ret, const char* configFilename, const GHXMLSerializer& xmlSerializer) const;
    
protected:
    GHEventMgr& mGameThreadEventMgr;
    GHMessageQueue& mGameThreadMessageQueue;

};
