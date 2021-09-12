// Copyright 2010 Golden Hammer Software
#include "GHiOSUIThreadSystemServices.h"
#include "GHiOSKeyboardTransitionXMLLoader.h"
#include "GHAppShard.h"

GHiOSUIThreadSystemServices::GHiOSUIThreadSystemServices(UIView* keyboardView,
                                                         GHiOSViewBehavior& viewBehavior,
                                                         GHMessageHandler& uiMessageQueue,
                                                         GHEventMgr& uiThreadEventMgr)
: mKeyboardView(keyboardView)
, mViewBehavior(viewBehavior)
, mUIMessageQueue(uiMessageQueue)
, mUIThreadEventMgr(uiThreadEventMgr)
{
}

void GHiOSUIThreadSystemServices::initAppShard(GHAppShard& appShard)
{
    GHiOSKeyboardTransitionXMLLoader* keyboardTrans = new GHiOSKeyboardTransitionXMLLoader(mKeyboardView, mViewBehavior, mUIMessageQueue, mUIThreadEventMgr);
    appShard.mXMLObjFactory.addLoader(keyboardTrans, 1, "showKeyboard");
}
