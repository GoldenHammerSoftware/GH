// Copyright 2010 Golden Hammer Software
#include "GHiOSIAPStoreFactory.h"
#include "GHiOSIAPStore.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"

GHiOSIAPStoreFactory::GHiOSIAPStoreFactory(GHEventMgr& gameThreadEventMgr, GHMessageQueue& gameThreadMessageQueue)
: GHAppleIAPStoreFactory(gameThreadEventMgr, gameThreadMessageQueue)
{
   
}

GHIAPStore* GHiOSIAPStoreFactory::createIAPStore(const char* configFilename,
                                                 const GHXMLSerializer& xmlSerializer) const
{
    GHiOSIAPStore* ret = new GHiOSIAPStore(mGameThreadEventMgr, mGameThreadMessageQueue);
    fillIAPStore(*ret, configFilename, xmlSerializer);
    return ret;
}
