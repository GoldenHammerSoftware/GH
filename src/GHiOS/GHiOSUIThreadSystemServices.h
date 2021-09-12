// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHServices.h"
#import <UIKit/UIKit.h>

class GHMessageHandler;
class GHEventMgr;
class GHiOSViewBehavior;

// A class to create things that won't exist until after the UI thread stuff is hooked up
// added to GHiOSSystemServices as a sub service.
class GHiOSUIThreadSystemServices : public GHServices
{
public:
    GHiOSUIThreadSystemServices(UIView* keyboardView, GHiOSViewBehavior& viewBehavior,
                                GHMessageHandler& uiMessageQueue,
                                GHEventMgr& uiThreadEventMgr);
    
    virtual void initAppShard(GHAppShard& appShard);

private:
    UIView* mKeyboardView;
    GHiOSViewBehavior& mViewBehavior;
    GHMessageHandler& mUIMessageQueue;
    GHEventMgr& mUIThreadEventMgr;
};
