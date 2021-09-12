// Copyright Golden Hammer Software
#include "GHAppShard.h"
#include "GHSystemServices.h"
#include "GHPlatform/GHDeletionHandle.h"

GHAppShard::GHAppShard(GHSystemServices& systemServices)
: mXMLObjFactory(systemServices.getPlatformServices().getIdFactory(), systemServices.getXMLSerializer()) 
, mResourceFactory(mResourceCache, systemServices.getPlatformServices().getIdFactory())
, mSystemServices(systemServices)
, mOwnedItems(new GHOwnershipList<GHDeletionHandle>)
{
    mSystemServices.getEventMgr().registerGlobalListener(mEventMgr);
}

GHAppShard::~GHAppShard(void)
{
    mSystemServices.getEventMgr().unregisterGlobalListener(mEventMgr);
    mStateMachine.setState(GHStateMachine::kInactive);
    delete mOwnedItems;
}

