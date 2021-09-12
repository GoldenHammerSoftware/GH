// Copyright Golden Hammer Software
#pragma once

#include "GHEntityMgr.h"
#include "GHEntityMgrMessageListener.h"

class GHConnectionHandle;
class GHAppShard;
class GHConnectionHandleController;

// interface for a client in a client/server
class GHCSClient
{
public:
    GHCSClient(GHAppShard& appShard);
    virtual ~GHCSClient(void);
    
    void connect(GHConnectionHandle* connection);
    void disconnect(void);
    
protected:
    virtual void onConnect(void) = 0;
    virtual void onDisconnect(void) = 0;
    
protected:
    GHAppShard& mAppShard;
    GHEntityMgr mEntityMgr;
    GHEntityMgrMessageListener mEntityMgrListener;
    
    GHConnectionHandle* mConnection;
    GHConnectionHandleController* mConnectionUpdater;
};
