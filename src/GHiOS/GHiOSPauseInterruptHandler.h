// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"

class GHEventMgr;
class GHiOSViewBehavior;

//hadles OS-level stuff for pause/unpause (eg input clearing)
class GHiOSPauseInterruptHandler : public GHMessageHandler
{
public:
    GHiOSPauseInterruptHandler(GHEventMgr& appEventMgr, GHEventMgr& systemEventMgr,
                               GHiOSViewBehavior& view);
    ~GHiOSPauseInterruptHandler(void);
    
    virtual void handleMessage(const GHMessage& message);
    
private:
    GHEventMgr& mAppEventMgr;
    GHEventMgr& mSystemEventMgr;
    GHiOSViewBehavior& mView;
};
