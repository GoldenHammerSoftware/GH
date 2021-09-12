// Copyright 2010 Golden Hammer Software
#include "GHiOSKeyboardTransitionXMLLoader.h"
#include "GHiOSKeyboardTransition.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessage.h"
#include "GHiOSIdentifiers.h"

GHiOSKeyboardTransitionXMLLoader::GHiOSKeyboardTransitionXMLLoader(UIView* keyboardView,
                                                                   GHiOSViewBehavior& viewBehavior,
                                                                   GHMessageHandler& uiMessageQueue,
                                                                   GHEventMgr& uiThreadEventMgr)
: mMessageListener(keyboardView, viewBehavior)
, mUIMessageQueue(uiMessageQueue)
, mUIThreadEventMgr(uiThreadEventMgr)
{
    mUIThreadEventMgr.registerListener(GHiOSIdentifiers::UISHOWKEYBOARD, mMessageListener);
    mUIThreadEventMgr.registerListener(GHiOSIdentifiers::UIHIDEKEYBOARD, mMessageListener);
}

GHiOSKeyboardTransitionXMLLoader::~GHiOSKeyboardTransitionXMLLoader(void)
{
    mUIThreadEventMgr.unregisterListener(GHiOSIdentifiers::UISHOWKEYBOARD, mMessageListener);
    mUIThreadEventMgr.unregisterListener(GHiOSIdentifiers::UIHIDEKEYBOARD, mMessageListener);
}

void* GHiOSKeyboardTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    return new GHiOSKeyboardTransition(mUIMessageQueue);
}

GHiOSKeyboardTransitionXMLLoader::MessageListener::MessageListener(UIView* keyboardView, GHiOSViewBehavior& viewBehavior)
: mKeyboardView(keyboardView)
, mViewBehavior(viewBehavior)
{
}

void GHiOSKeyboardTransitionXMLLoader::MessageListener::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHiOSIdentifiers::UISHOWKEYBOARD)
    {
        mViewBehavior.setAllowKeyboard(true);
        [mKeyboardView becomeFirstResponder];
        mViewBehavior.setAllowKeyboard(false);
    }
    else if (message.getType() == GHiOSIdentifiers::UIHIDEKEYBOARD)
    {
        [mKeyboardView resignFirstResponder];
    }
}
