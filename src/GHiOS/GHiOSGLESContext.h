// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGLESContext.h"
#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>
#include "GHUtils/GHMessageHandler.h"
// no ogl include!

class GHEventMgr;

class GHiOSGLESContext : public GHGLESContext
{
public:
	GHiOSGLESContext(CAEAGLLayer* aglLayer, EAGLContext* context, bool msAllowed,
                     GHEventMgr& eventMgr);
	virtual ~GHiOSGLESContext(void);
    
	virtual void beginFrame(void);
	virtual void endFrame(void);

    void handleWindowSizeChanged(void);
    
private:
    void initRenderBufferStorage(void);
	void destroyRenderBufferStorage(void);

private:
    class WindowSizeListener : public GHMessageHandler
    {
    public:
        WindowSizeListener(GHiOSGLESContext& parent, GHEventMgr& eventMgr);
        ~WindowSizeListener(void);
        
        virtual void handleMessage(const GHMessage& message);

    private:
        GHEventMgr& mEventMgr;
        GHiOSGLESContext& mParent;
    };
    
private:
    WindowSizeListener mWindowSizeListener;
	EAGLContext* mContext;
	CAEAGLLayer* mAGLLayer;
	unsigned int mViewRenderbuffer;
	unsigned int mViewFramebuffer;
	unsigned int mDepthRenderbuffer;
	int mBackingWidth;
	int mBackingHeight;
	
	bool mSupportsAntiAliasing;
	unsigned int mMSAAFramebuffer;
	unsigned int mMSAARenderbuffer;
	unsigned int mMSAADepthBuffer;
    
    // we set this when the window changed.
    // a flag to call destroyRenderBufferStorage and initRenderBufferStorage
    bool mBuffersNeedRecreate;
};
