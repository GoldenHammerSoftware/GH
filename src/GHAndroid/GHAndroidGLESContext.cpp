// Copyright 2010 Golden Hammer Software
#include "GHAndroidGLESContext.h"
#include <stdlib.h>
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHEventMgr.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHMessage.h"
#include "GHRenderProperties.h"
#include "GHGLESErrorReporter.h"
#include "GHGLESInclude.h"
#include "GHScreenInfo.h"
#include "GHGLESReinitializer.h"

GHAndroidGLESContext::GHAndroidGLESContext(GHEventMgr& eventMgr, GHScreenInfo& screenInfo, GHGLESReinitializer& reinitializer)
: mInterruptHandler(eventMgr, *this)
, mReinitNeeded(false)
, mEventMgr(eventMgr)
, mScreenInfo(screenInfo)
, mReinitializer(reinitializer)
{
    
}

GHAndroidGLESContext::~GHAndroidGLESContext(void)
{
    
}

void GHAndroidGLESContext::beginFrame(void)
{
    if (mReinitNeeded)
    {
        glViewport (0, 0, mScreenInfo.getSize()[0], mScreenInfo.getSize()[1]);
        GHGLESErrorReporter::checkAndReportError("glViewport");
        mReinitializer.handleDeviceReinit();
        mReinitNeeded = false;
    }
}

void GHAndroidGLESContext::endFrame(void)
{

}

void GHAndroidGLESContext::handleDeviceLost(void)
{
    mReinitializer.handleDeviceLost();
}

void GHAndroidGLESContext::setReinitNeeded(void)
{
    GHDebugMessage::outputString("Setting reinit needed");
    mReinitNeeded = true;
}

void GHAndroidGLESContext::handleGraphicsQualityChanged(void)
{
    mReinitializer.handleGraphicsQualityChanged();
}

void GHAndroidGLESContext::onResizeWindow(void)
{
    glViewport (0, 0, mScreenInfo.getSize()[0], mScreenInfo.getSize()[1]);
}

GHAndroidGLESContext::InterruptHandler::InterruptHandler(GHEventMgr& eventMgr,
                                                         GHAndroidGLESContext& parent)
: mEventMgr(eventMgr)
, mParent(parent)
{
    mEventMgr.registerListener(GHMessageTypes::PAUSEINTERRUPT, *this);
    mEventMgr.registerListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
    mEventMgr.registerListener(GHMessageTypes::WINDOWRESIZE, *this);
}

GHAndroidGLESContext::InterruptHandler::~InterruptHandler(void)
{
    mEventMgr.unregisterListener(GHMessageTypes::WINDOWRESIZE, *this);
    mEventMgr.unregisterListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
    mEventMgr.unregisterListener(GHMessageTypes::PAUSEINTERRUPT, *this);
}

void GHAndroidGLESContext::InterruptHandler::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHMessageTypes::PAUSEINTERRUPT)
    {
		mPauseHappened = true;
        mParent.handleDeviceLost();
    }
    else if (message.getType() == GHMessageTypes::UNPAUSEINTERRUPT)
    {
		if (mPauseHappened)
		{
			GHDebugMessage::outputString("Handling unpause interrupt in GHAndroidGLESContext");
			mParent.setReinitNeeded();
			mPauseHappened = false;
		}
    }
    else if (message.getType() == GHMessageTypes::WINDOWRESIZE)
    {
        mParent.onResizeWindow();
    }
}
