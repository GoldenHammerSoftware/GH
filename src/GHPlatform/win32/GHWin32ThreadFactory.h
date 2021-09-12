// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHThreadFactory.h"

class GHWin32ThreadFactory : public GHThreadFactory
{
public:
	virtual GHThread* createThread(GHThread::Priority pri) const;
	virtual GHMutex* createMutex(void) const;
	virtual GHThreadSleeper* createSleeper(void) const;
	virtual GHThreadSignal* createSignal(void) const;
};
