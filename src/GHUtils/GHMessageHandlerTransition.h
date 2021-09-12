// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include <vector>
#include "GHString/GHIdentifier.h"

class GHEventMgr;
class GHMessageHandler;

// A transition for adding/removing message handlers during a state.
class GHMessageHandlerTransition : public GHTransition
{
public:
    GHMessageHandlerTransition(GHEventMgr& eventMgr, GHIdentifier message, GHMessageHandler* messageHandler);
    GHMessageHandlerTransition(GHEventMgr& eventMgr);
    
    ~GHMessageHandlerTransition(void);
	
	virtual void activate(void);
	virtual void deactivate(void);
    
    void addHandler(GHIdentifier message, GHMessageHandler* handler);
    void addHandler(const std::vector<GHIdentifier>& messages, GHMessageHandler* handler);
    
private:
    struct MessageHandler
    {
        std::vector<GHIdentifier> mMessages;
        GHMessageHandler* mHandler;
    };
    
    GHEventMgr& mEventMgr;
    std::vector<MessageHandler> mMessageHandlers;
};
