// Copyright Golden Hammer Software
#pragma once

#include "GHConnectionHandle.h"
#include <vector>

class GHCSClientInfo;

// class for managing a list of clients from a server
class GHCSClientList : public GHConnectionHandle
{
public:
    GHCSClientList(void);
    ~GHCSClientList(void);
    
    GHCSClientInfo* connect(GHConnectionHandle* connection);
    void disconnect(GHConnectionHandle& connection);
    void disconnectAll(void);
    
    const std::vector<GHCSClientInfo*>& getClients(void) { return mClients; }
    GHCSClientInfo* getClient(GHConnectionHandle& connection);
    
    virtual void sendReliable(const GHMessage& message);
    virtual void sendUnreliable(const GHMessage& message);
    virtual void readMessages(GHMessageHandler& handler);

private:
    std::vector<GHCSClientInfo*> mClients;
    int mNextClientId;
};
