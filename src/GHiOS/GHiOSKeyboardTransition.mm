// Copyright 2010 Golden Hammer Software
#include "GHiOSKeyboardTransition.h"
#include "GHiOSIdentifiers.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHMessageHandler.h"

GHiOSKeyboardTransition::GHiOSKeyboardTransition(GHMessageHandler& uiMessageQueue)
: mUIMessageQueue(uiMessageQueue)
{

}

GHiOSKeyboardTransition::~GHiOSKeyboardTransition(void)
{

}

void GHiOSKeyboardTransition::activate(void)
{
    GHMessage message(GHiOSIdentifiers::UISHOWKEYBOARD);
    mUIMessageQueue.handleMessage(message);
}

void GHiOSKeyboardTransition::deactivate(void)
{
    GHMessage message(GHiOSIdentifiers::UIHIDEKEYBOARD);
    mUIMessageQueue.handleMessage(message);
}
