// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHThread.h"

class GHJNIMgr;

class GHAndroidThreadFactory : public GHThreadFactory
{
public:
	GHAndroidThreadFactory(GHJNIMgr& jniMgr);

	virtual GHThread* createThread(GHThread::Priority pri) const;
	virtual GHMutex* createMutex(void) const;
	virtual GHThreadSleeper* createSleeper(void) const;
	virtual GHThreadSignal* createSignal(void) const;

private:
	GHJNIMgr& mJNIMgr;
};
