// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGLESContext.h"
//#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "GHUtils/GHMessageHandler.h"

class GHEventMgr;
class GHScreenInfo;
class GHGLESReinitializer;

class GHAndroidGLESContext : public GHGLESContext
{
public:
	GHAndroidGLESContext(GHEventMgr& eventMgr, GHScreenInfo& screenInfo, GHGLESReinitializer& reinitializer);
	~GHAndroidGLESContext(void);

	virtual void beginFrame(void);
	virtual void endFrame(void);
    
    void handleDeviceLost(void);
    virtual void setReinitNeeded(void);
    virtual void handleGraphicsQualityChanged(void);
    void onResizeWindow(void);
    
private:
    bool mReinitNeeded;
    GHEventMgr& mEventMgr;
    class InterruptHandler : public GHMessageHandler
    {
    public:
        InterruptHandler(GHEventMgr& eventMgr,
                         GHAndroidGLESContext& parent);
        ~InterruptHandler(void);
        virtual void handleMessage(const GHMessage& message);
    private:
        GHEventMgr& mEventMgr;
        GHAndroidGLESContext& mParent;
		bool mPauseHappened{ false };
    };
    InterruptHandler mInterruptHandler;
    GHScreenInfo& mScreenInfo;
    GHGLESReinitializer& mReinitializer;
    
};
