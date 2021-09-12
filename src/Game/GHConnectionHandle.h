// Copyright Golden Hammer Software
#pragma once

class GHMessage;
class GHMessageHandler;

// interface for sending messages across a network.
// assume all message props must be fixed size and not pointers.
class GHConnectionHandle
{
public:
    virtual ~GHConnectionHandle(void) {}
    
    // send a tcp message
    virtual void sendReliable(const GHMessage& message) = 0;
    // send a udp message.
    virtual void sendUnreliable(const GHMessage& message) = 0;
    // caller is responsible for deleting the message.
    virtual void readMessages(GHMessageHandler& handler) = 0;
};
