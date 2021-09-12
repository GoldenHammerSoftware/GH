// Copyright 2010 Golden Hammer Software
#include "GHCocoaIAPStoreFactory.h"
#include "GHCocoaIAPStore.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"

GHCocoaIAPStoreFactory::GHCocoaIAPStoreFactory(GHEventMgr& gameThreadEventMgr, GHMessageQueue& gameThreadMessageQueue)
: GHAppleIAPStoreFactory(gameThreadEventMgr, gameThreadMessageQueue)
{
    
}

GHIAPStore* GHCocoaIAPStoreFactory::createIAPStore(const char* configFilename,
                                                   const GHXMLSerializer& xmlSerializer) const
{
    GHCocoaIAPStore* ret = new GHCocoaIAPStore(mGameThreadEventMgr, mGameThreadMessageQueue);
    fillIAPStore(*ret, configFilename, xmlSerializer);
    return ret;
}
