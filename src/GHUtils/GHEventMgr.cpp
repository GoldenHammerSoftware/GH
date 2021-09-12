// Copyright Golden Hammer Software
#include "GHEventMgr.h"
#include "GHMessage.h"
#include <algorithm>
#include <cassert>

GHEventMgr::GHEventMgr(void)
: mBroadcastingCount(0)
{
}

void GHEventMgr::registerListener(GHIdentifier type, GHMessageHandler& handler)
{
	if (!mBroadcastingCount)
	{
		mEventHandlers[type].push_back(&handler);
	}
	else 
	{
		DelayedListenerChange change;
		change.mAdd = true;
		change.mMessageType = type;
		change.mEventHandler = &handler;
		mDelayedChanges.push_back(change);
	}
}

void GHEventMgr::unregisterListener(GHIdentifier type, GHMessageHandler& handler)
{
	if (!mBroadcastingCount)
	{
		EventHandlerVector& listeners = mEventHandlers[type];
		EventHandlerVector::iterator findIter;
		findIter = std::find(listeners.begin(), listeners.end(), &handler);
		if (findIter != listeners.end()) {
			listeners.erase(findIter);
		}
	}
	else 
	{
		DelayedListenerChange change;
		change.mAdd = false;
		change.mMessageType = type;
		change.mEventHandler = &handler;
		mDelayedChanges.push_back(change);
	}
}

void GHEventMgr::registerGlobalListener(GHMessageHandler& handler)
{
	if (!mBroadcastingCount)
	{
		mGlobalHandlers.push_back(&handler);
	}
	else 
	{
		DelayedListenerChange change;
		change.mAdd = true;
		change.mGlobal = true;
		change.mEventHandler = &handler;
		mDelayedChanges.push_back(change);
	}
}

void GHEventMgr::unregisterGlobalListener(GHMessageHandler& handler)
{
	if (!mBroadcastingCount)
	{
		EventHandlerVector::iterator findIter;
		findIter = std::find(mGlobalHandlers.begin(), mGlobalHandlers.end(), &handler);
		if (findIter != mGlobalHandlers.end()) {
			mGlobalHandlers.erase(findIter);
		}
	}
	else 
	{
		DelayedListenerChange change;
		change.mAdd = false;
        change.mGlobal = true;
		change.mEventHandler = &handler;
		mDelayedChanges.push_back(change);
	}
}

void GHEventMgr::handleMessage(const GHMessage& message)

{
	++mBroadcastingCount;
	
	EventHandlerVector& listeners = mEventHandlers[message.getType()];
	EventHandlerVector::iterator iter;
	for (iter = listeners.begin(); iter != listeners.end(); ++iter) {
		(*iter)->handleMessage(message);
	}
    for (iter = mGlobalHandlers.begin(); iter != mGlobalHandlers.end(); ++iter) {
		(*iter)->handleMessage(message);
    }
	
	--mBroadcastingCount;
    assert(mBroadcastingCount >= 0);
	
    if (mBroadcastingCount == 0)
    {
        std::vector<DelayedListenerChange>::iterator delayedIter;
        for (delayedIter = mDelayedChanges.begin(); delayedIter != mDelayedChanges.end(); ++delayedIter)
        {
            if (delayedIter->mAdd) {
                if (!delayedIter->mGlobal) {
                    registerListener(delayedIter->mMessageType, *delayedIter->mEventHandler);
                }
                else {
                    registerGlobalListener(*delayedIter->mEventHandler);
                }
            }
            else {
                if (!delayedIter->mGlobal) {
                    unregisterListener(delayedIter->mMessageType, *delayedIter->mEventHandler);
                }
                else {
                    unregisterGlobalListener(*delayedIter->mEventHandler);
                }
            }
        }
        mDelayedChanges.clear();
    }
}

