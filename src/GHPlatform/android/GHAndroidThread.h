// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHThread.h"
#include <pthread.h>

class GHJNIMgr;

class GHAndroidThread : public GHThread
{
public:
	GHAndroidThread(GHJNIMgr& jniMgr);

	void runThread(GHRunnable& runnable);

private:
	GHJNIMgr& mJNIMgr;
};
