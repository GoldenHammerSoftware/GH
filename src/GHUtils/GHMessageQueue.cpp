// Copyright Golden Hammer Software
#include "GHMessageQueue.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHMutex.h"
#include "GHMessage.h"
#include "GHPlatform/GHStlUtil.h"
#include "GHPlatform/GHDebugMessage.h"

GHMessageQueue::GHMessageQueue(const GHThreadFactory* threadFactory)
: mWaitlistMutex(0)
, mQueueMutex(0)
, mIsSending(false)
{
    if (threadFactory) mQueueMutex = threadFactory->createMutex();
    if (threadFactory) mWaitlistMutex = threadFactory->createMutex();
}

GHMessageQueue::~GHMessageQueue(void)
{
    delete mWaitlistMutex;
    delete mQueueMutex;
}

void GHMessageQueue::handleMessage(const GHMessage& message)
{
    if (mIsSending)
    {
        mWaitlistMutex->acquire();
        mWaitlist.push_back(message);
        mWaitlistMutex->release();
        return;
    }
    
    if (mQueueMutex) mQueueMutex->acquire();
    mMessages.push_back(message);
    if (mQueueMutex) mQueueMutex->release();
}

void GHMessageQueue::sendMessages(GHMessageHandler& handler)
{
    if (mQueueMutex) mQueueMutex->acquire();
    mIsSending = true;
    
    for (size_t i = 0; i < mMessages.size(); ++i)
    {
		//GHDebugMessage::outputString("Sending queued message");
        handler.handleMessage(mMessages[i]);
    }
    mMessages.clear();
    
    mWaitlistMutex->acquire();
    mMessages = mWaitlist;
    mWaitlist.clear();
    mWaitlistMutex->release();
    
    mIsSending = false;
    if (mQueueMutex) mQueueMutex->release();
}

void GHMessageQueue::clearMessages(void)
{
	if (mQueueMutex) mQueueMutex->acquire();
	mMessages.clear();
	mWaitlist.clear();
	if (mQueueMutex) mQueueMutex->release();
}
