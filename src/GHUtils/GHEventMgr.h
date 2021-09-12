// Copyright Golden Hammer Software
#pragma once

#include "GHMessageHandler.h"
#include "GHString/GHIdentifier.h"
#include <vector>
#include <map>

class GHEventMgr : public GHMessageHandler
{
public:
	GHEventMgr(void);
	
    // register for specific message types
	void registerListener(GHIdentifier type, GHMessageHandler& handler);
	void unregisterListener(GHIdentifier type, GHMessageHandler& handler);
	// register for all message types at once
    void registerGlobalListener(GHMessageHandler& handler);
    void unregisterGlobalListener(GHMessageHandler& handler);
    
	virtual void handleMessage(const GHMessage& message);
	
protected:
	typedef std::vector<GHMessageHandler*> EventHandlerVector;
    EventHandlerVector mGlobalHandlers;
	std::map<GHIdentifier, EventHandlerVector> mEventHandlers;
	int mBroadcastingCount;
	
	class DelayedListenerChange
	{
    public:
        DelayedListenerChange(void) : mGlobal(false) {}
        
		GHIdentifier mMessageType;
		GHMessageHandler* mEventHandler{ 0 };
		bool mAdd{ true };
		bool mGlobal{ false };
	};
	std::vector<DelayedListenerChange> mDelayedChanges;
};
