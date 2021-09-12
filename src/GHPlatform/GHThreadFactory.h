#pragma once

#include "GHThread.h"

class GHMutex;
class GHThreadSleeper;
class GHThreadSignal;

/// Interface for platform abstraction on threads.
class GHThreadFactory
{
public:
	virtual ~GHThreadFactory(void) {}
	
	virtual GHThread* createThread(GHThread::Priority pri) const = 0;
	virtual GHMutex* createMutex(void) const = 0;
	virtual GHThreadSleeper* createSleeper(void) const = 0;
	virtual GHThreadSignal* createSignal(void) const = 0;
};
