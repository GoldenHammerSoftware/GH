// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjFactory.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHStateMachine.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHUtils/GHPropertyContainer.h"
#include <vector>
#include "GHPlatform/GHOwnershipList.h"
#include "GHUtils/GHResourceCache.h"

class GHSystemServices;
class GHDeletionHandle;

// Base class for client and server
// A chunk of app that runs by itself and has the necessary stuff
class GHAppShard
{
public:
    GHAppShard(GHSystemServices& systemServices);
    virtual ~GHAppShard(void);

    void addOwnedItem(GHDeletionHandle* handle) { mOwnedItems->push_back(handle); }
    
public:
    GHSystemServices& mSystemServices;
    GHResourceCache mResourceCache;
    GHXMLObjFactory mXMLObjFactory;
    GHResourceFactory mResourceFactory;
    GHEventMgr mEventMgr;
    GHPropertyContainer mProps;
    GHStateMachine mStateMachine;
    GHControllerMgr mControllerMgr;
    // mOwnedItems is a pointer so we delete it before our members.
    GHOwnershipList<GHDeletionHandle>* mOwnedItems;
};
