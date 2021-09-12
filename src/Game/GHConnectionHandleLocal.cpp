// Copyright Golden Hammer Software
#include "GHConnectionHandleLocal.h"
#include <assert.h>

GHConnectionHandleLocal::GHConnectionHandleLocal(const GHThreadFactory* threadFactory)
: mTarget(0)
, mIncomingMessages(threadFactory)
{
}

GHConnectionHandleLocal::~GHConnectionHandleLocal(void)
{
}

void GHConnectionHandleLocal::sendReliable(const GHMessage& message)
{
    assert(mTarget);
    mTarget->getIncomingQueue().handleMessage(message);
}

void GHConnectionHandleLocal::sendUnreliable(const GHMessage& message)
{
    assert(mTarget);
    mTarget->getIncomingQueue().handleMessage(message);
}

void GHConnectionHandleLocal::readMessages(GHMessageHandler& handler)
{
    mIncomingMessages.sendMessages(handler);
}

void GHConnectionHandleLocal::setTarget(GHConnectionHandleLocal* target)
{
    mTarget = target;
}

GHMessageQueue& GHConnectionHandleLocal::getIncomingQueue(void)
{
    return mIncomingMessages;
}
