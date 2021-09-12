// Copyright Golden Hammer Software
#include "GHWin32ThreadFactory.h"
#include "GHWin32Thread.h"
#include "GHWin32Mutex.h"
#include "GHWin32ThreadSleeper.h"
#include "GHPlatform\null\GHNullThreadSignal.h"

GHThread* GHWin32ThreadFactory::createThread(GHThread::Priority pri) const
{
	// todo: support priority.
	return new GHWin32Thread;
}

GHMutex* GHWin32ThreadFactory::createMutex(void) const
{
	return new GHWin32Mutex;
}

GHThreadSleeper* GHWin32ThreadFactory::createSleeper(void) const
{
	return new GHWin32ThreadSleeper;
}

GHThreadSignal* GHWin32ThreadFactory::createSignal(void) const
{
	// todo.
	return new GHNullThreadSignal();
}