// Copyright Golden Hammer Software
#include "GHInterruptableTransition.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"

GHInterruptableTransition::GHInterruptableTransition(GHEventMgr& eventMgr, GHTransition* trans)
: mMessageHandler(*this)
, mEventMgr(eventMgr)
, mIsInterruptPaused(false)
, mIsActive(false)
{
    mTransitions.push_back(trans);
    mEventMgr.registerListener(GHMessageTypes::PAUSEINTERRUPT, mMessageHandler);
    mEventMgr.registerListener(GHMessageTypes::UNPAUSEINTERRUPT, mMessageHandler);
}

GHInterruptableTransition::~GHInterruptableTransition(void)
{
    mEventMgr.unregisterListener(GHMessageTypes::UNPAUSEINTERRUPT, mMessageHandler);
    mEventMgr.unregisterListener(GHMessageTypes::PAUSEINTERRUPT, mMessageHandler);
    
	for (size_t i = 0; i < mTransitions.size(); ++i) {
        delete mTransitions[i];
    }
}

void GHInterruptableTransition::activate(void)
{
    if (!mIsInterruptPaused)
    {
        activateTransitions();
    }
    mIsActive = true;
}

void GHInterruptableTransition::deactivate(void)
{
    if (!mIsInterruptPaused)
    {
        deactivateTransitions();
    }
    mIsActive = false;
}

void GHInterruptableTransition::handlePauseInterrupt(void)
{
    if (mIsActive)
    {
        deactivateTransitions();
    }
    mIsInterruptPaused = true;
}

void GHInterruptableTransition::handleUnpauseInterrupt(void)
{
    if (mIsActive)
    {
        activateTransitions();
    }
    mIsInterruptPaused = false;
}

void GHInterruptableTransition::activateTransitions(void)
{
	for (size_t i = 0; i < mTransitions.size(); ++i) {
        mTransitions[i]->activate();
    }
}

void GHInterruptableTransition::deactivateTransitions(void)
{
	for (size_t i = 0; i < mTransitions.size(); ++i) {
        mTransitions[i]->deactivate();
    }
}

GHInterruptableTransition::MessageHandler::MessageHandler(GHInterruptableTransition& parent)
: mParent(parent)
{
    
}
    
void GHInterruptableTransition::MessageHandler::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHMessageTypes::PAUSEINTERRUPT)
    {
        mParent.handlePauseInterrupt();
    }
    else if (message.getType() == GHMessageTypes::UNPAUSEINTERRUPT)
    {
        mParent.handleUnpauseInterrupt();
    }
}
