// Copyright Golden Hammer Software
#pragma once

#include "GHCSClientList.h"
#include "GHEntityMgr.h"

class GHConnectionHandle;
class GHString;
class GHAppShard;

// interface for a sever that talks to multiple clients.
class GHCSServer
{
public:
    GHCSServer(GHAppShard& appShard);
    virtual ~GHCSServer(void);
    
    void connect(GHConnectionHandle* connection);
    void disconnect(GHConnectionHandle& connection);
    void disconnectAll(void);
    
    virtual void setLevel(const GHString& level) = 0;
    
protected:
    virtual void onConnected(GHCSClientInfo& client) = 0;
    virtual void onDisconnected(GHCSClientInfo& client) = 0;
    
protected:
    GHCSClientList mClients;
    GHEntityMgr mEntityMgr;
};
