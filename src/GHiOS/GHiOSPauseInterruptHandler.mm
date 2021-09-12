// Copyright 2010 Golden Hammer Software
#include "GHiOSPauseInterruptHandler.h"
#include "GHUtils/GHEventMgr.h"
#include "GHMessageTypes.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHMessage.h"
#include "GHiOSViewBehavior.h"

GHiOSPauseInterruptHandler::GHiOSPauseInterruptHandler(GHEventMgr& appEventMgr, GHEventMgr& systemEventMgr,
                                                       GHiOSViewBehavior& view)
: mAppEventMgr(appEventMgr)
, mSystemEventMgr(systemEventMgr)
, mView(view)
{
    mAppEventMgr.registerListener(GHMessageTypes::PAUSEINTERRUPT, *this);
    mAppEventMgr.registerListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
}

GHiOSPauseInterruptHandler::~GHiOSPauseInterruptHandler(void)
{
    mAppEventMgr.unregisterListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
    mAppEventMgr.unregisterListener(GHMessageTypes::PAUSEINTERRUPT, *this);
}

void GHiOSPauseInterruptHandler::handleMessage(const GHMessage& message)
{
    mView.clearInput();
    
    // force a renderer restart to avoid a bug where gl gets detached.
    GHMessage resizeMessage(GHMessageTypes::WINDOWRESIZE);
    mSystemEventMgr.handleMessage(resizeMessage);
}
