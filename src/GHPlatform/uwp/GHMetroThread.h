// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHThread.h"
#include "GHPlatform/GHRunnable.h"

class GHMetroThread : public GHThread
{
public:
	void runThread(GHRunnable& runnable);

private:
	// we need a member variable to refer to runnable from the threading code.
	// we assume 1 thread runs 1 runnable at a time.
	GHRunnable* mRunnable;
};
