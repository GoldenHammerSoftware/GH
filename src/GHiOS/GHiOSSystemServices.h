// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSystemServices.h"

class GHiOSSoundDevice;
class GHMessageQueue;
class GHiOSPauseInterruptHandler;
class GHiOSViewBehavior;

class GHiOSSystemServices : public GHSystemServices
{
public:
    GHiOSSystemServices(GHiOSViewBehavior& viewBehavior);
    ~GHiOSSystemServices(void);
    
    virtual void initAppShard(GHAppShard& appShard);
    
private:
    GHiOSSoundDevice* miOSSoundDevice;
    GHiOSViewBehavior& mViewBehavior;
    GHiOSPauseInterruptHandler* mPauseInterruptHandler;
};
