// Copyright 2010 Golden Hammer Software
#pragma once

#import <UIKit/UIKit.h>
#include "GHiOSViewBehavior.h"
#include "GHXMLObjLoader.h"
#include "GHUtils/GHMessageHandler.h"

class GHEventMgr;

class GHiOSKeyboardTransitionXMLLoader : public GHXMLObjLoader
{
public:
    GHiOSKeyboardTransitionXMLLoader(UIView* keyboardView, GHiOSViewBehavior& viewBehavior,
                                     GHMessageHandler& uiMessageQueue,
                                     GHEventMgr& uiThreadEventMgr);
    ~GHiOSKeyboardTransitionXMLLoader(void);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    class MessageListener : public GHMessageHandler
    {
    public:
        MessageListener(UIView* keyboardView, GHiOSViewBehavior& viewBehavior);
        virtual void handleMessage(const GHMessage& message);
    private:
        UIView* mKeyboardView;
        GHiOSViewBehavior& mViewBehavior;
    } mMessageListener;

private:
    GHMessageHandler& mUIMessageQueue;
    GHEventMgr& mUIThreadEventMgr;
};
