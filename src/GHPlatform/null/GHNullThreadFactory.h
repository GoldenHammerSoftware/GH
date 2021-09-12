// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHThreadFactory.h"

/// Interface for platform abstraction on threads.
class GHNullThreadFactory : public GHThreadFactory
{
public:
	virtual GHThread* createThread(GHThread::Priority pri) const;
	virtual GHMutex* createMutex(void) const;
	virtual GHThreadSleeper* createSleeper(void) const;
	virtual GHThreadSignal* createSignal(void) const;
};
