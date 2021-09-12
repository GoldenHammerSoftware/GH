// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHOGLContext.h"
#include "GHMessageHandler.h"

class GHEventMgr;
class GHScreenInfo;

class GHCocoaOGLContext : public GHOGLContext
{
public:
    GHCocoaOGLContext(NSView* view, GHEventMgr& eventMgr, GHScreenInfo& screenInfo);
    ~GHCocoaOGLContext(void);
    
	virtual void beginFrame(void);
	virtual void swapBuffers(void);
    virtual void setFullScreen(bool isFull);
    
protected:
    void updateViewPort(void);

private:
    class MessageHandler : public GHMessageHandler
    {
    public:
        MessageHandler(GHCocoaOGLContext& context);
        virtual void handleMessage(const GHMessage& message);
    private:
        GHCocoaOGLContext& mContext;
    };
    friend class GHCocoaOGLContext::MessageHandler;
    
private:
    NSView* mView;
	NSOpenGLPixelFormat* mPixelFormat;
	NSOpenGLContext* mOGLContext;
    GHScreenInfo& mScreenInfo;
    
    GHEventMgr& mEventMgr;
    MessageHandler mMessageHandler;
};
