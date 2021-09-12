// Copyright Golden Hammer Software
#pragma once

#include "GHConnectionHandle.h"
#include "GHUtils/GHMessageQueue.h"

class GHThreadFactory;

// a connection handle that does not go over the network.
// it just communicates with another local connection handle.
class GHConnectionHandleLocal : public GHConnectionHandle
{
public:
    GHConnectionHandleLocal(const GHThreadFactory* threadFactory);
    ~GHConnectionHandleLocal(void);
    
    virtual void sendReliable(const GHMessage& message);
    virtual void sendUnreliable(const GHMessage& message);
    virtual void readMessages(GHMessageHandler& handler);
    
    void setTarget(GHConnectionHandleLocal* target);
    GHMessageQueue& getIncomingQueue(void);
    
private:
    GHMessageQueue mIncomingMessages;
    GHConnectionHandleLocal* mTarget;
};
