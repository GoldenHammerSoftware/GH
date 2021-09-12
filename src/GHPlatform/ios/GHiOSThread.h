// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHThread.h"

class GHiOSThread : public GHThread
{
public:
    GHiOSThread(float pri);
    
    virtual void runThread(GHRunnable& runnable);
    
private:
    float mPri;
};
