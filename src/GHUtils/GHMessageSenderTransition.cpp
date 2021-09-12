// Copyright Golden Hammer Software
#include "GHUtils/GHMessageSenderTransition.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHUtils/GHMessage.h"
#include <assert.h>

GHMessageSenderTransition::GHMessageSenderTransition(GHMessageHandler& handler)
: mMessageHandler(handler)
{
}

GHMessageSenderTransition::~GHMessageSenderTransition(void)
{
    for (size_t i = 0; i < mActivateMessages.size(); ++i)
    {
        delete mActivateMessages[i];
    }
    for (size_t i = 0; i < mDeactivateMessages.size(); ++i)
    {
        delete mDeactivateMessages[i];
    }
}

void GHMessageSenderTransition::activate(void)
{
    for (size_t i = 0; i < mActivateMessages.size(); ++i)
    {
        mMessageHandler.handleMessage(*mActivateMessages[i]);
    }
}

void GHMessageSenderTransition::deactivate(void)
{
    for (size_t i = 0; i < mDeactivateMessages.size(); ++i)
    {
        mMessageHandler.handleMessage(*mDeactivateMessages[i]);
    }
}

void GHMessageSenderTransition::addActivateMessage(GHMessage* message)
{
    mActivateMessages.push_back(message);
}

void GHMessageSenderTransition::addDeactivateMessage(GHMessage* message)
{
    mDeactivateMessages.push_back(message);
}
