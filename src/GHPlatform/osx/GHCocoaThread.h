// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHThread.h"

class GHCocoaThread : public GHThread
{
public:
    virtual void runThread(GHRunnable& runnable);
};
