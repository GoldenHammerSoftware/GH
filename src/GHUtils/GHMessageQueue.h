// Copyright Golden Hammer Software
#pragma once

#include "GHMessageHandler.h"
#include <vector>

class GHThreadFactory;
class GHMutex;

// A thread safe way to store up messages for later sending.
class GHMessageQueue : public GHMessageHandler
{
public:
    // Thread factory is optional.  We are not thread safe if it is not passed in.
    GHMessageQueue(const GHThreadFactory* threadFactory);
    ~GHMessageQueue(void);
    
    // store a message for later.
	virtual void handleMessage(const GHMessage& message);
    // send all queued messages to the handler,
    //  and clear out our list.
    void sendMessages(GHMessageHandler& handler);
	// clear out our messages without sending any.
	void clearMessages(void);
    
private:
    std::vector<GHMessage> mMessages;
    std::vector<GHMessage> mWaitlist;
    GHMutex* mQueueMutex;
    GHMutex* mWaitlistMutex;
    bool mIsSending;
};
