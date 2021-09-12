// Copyright Golden Hammer Software
#include "GHMessageHandlerTransition.h"
#include "GHUtils/GHEventMgr.h"
#include <string.h>

GHMessageHandlerTransition::GHMessageHandlerTransition(GHEventMgr& eventMgr, GHIdentifier message, GHMessageHandler* messageHandler)
: mEventMgr(eventMgr)
{
    addHandler(message, messageHandler);
}

GHMessageHandlerTransition::GHMessageHandlerTransition(GHEventMgr& eventMgr)
: mEventMgr(eventMgr)
{
    
}

GHMessageHandlerTransition::~GHMessageHandlerTransition(void)
{
    deactivate();
    size_t numHandlers = mMessageHandlers.size();
    for (size_t i = 0; i < numHandlers; ++i)
    {
        MessageHandler& structRef = mMessageHandlers[i];
        delete structRef.mHandler;
    }
}

void GHMessageHandlerTransition::activate(void)
{
    size_t numHandlers = mMessageHandlers.size();
    for (size_t i = 0; i < numHandlers; ++i)
    {
        MessageHandler& structRef = mMessageHandlers[i];
        size_t numMessages = structRef.mMessages.size();
        for (size_t msg = 0; msg < numMessages; ++msg)
        {
            mEventMgr.registerListener(structRef.mMessages[msg], *structRef.mHandler);
        }
    }
}

void GHMessageHandlerTransition::deactivate(void)
{
    int numHandlers = (int)mMessageHandlers.size();
    for (int i = numHandlers-1; i >= 0; --i)
    {
        MessageHandler& structRef = mMessageHandlers[i];
        int numMessages = (int)structRef.mMessages.size();
        for (int msg = numMessages-1; msg >= 0; --msg)
        {
            mEventMgr.unregisterListener(structRef.mMessages[msg], *structRef.mHandler);
        }
    }   
}

void GHMessageHandlerTransition::addHandler(GHIdentifier message, GHMessageHandler* handler)
{
    mMessageHandlers.push_back(MessageHandler());
    mMessageHandlers.back().mHandler = handler;
    mMessageHandlers.back().mMessages.push_back(message);
}

void GHMessageHandlerTransition::addHandler(const std::vector<GHIdentifier>& messages, GHMessageHandler* handler)
{
    mMessageHandlers.push_back(MessageHandler());
    mMessageHandlers.back().mHandler = handler;
    mMessageHandlers.back().mMessages = messages;
}
