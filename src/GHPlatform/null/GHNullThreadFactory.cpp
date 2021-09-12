// Copyright Golden Hammer Software
#include "GHNullThreadFactory.h"
#include "GHNullThread.h"
#include "GHNullMutex.h"
#include "GHNullThreadSleeper.h"
#include "GHNullThreadSignal.h"

GHThread* GHNullThreadFactory::createThread(GHThread::Priority pri) const
{
	return new GHNullThread();
}

GHMutex* GHNullThreadFactory::createMutex(void) const
{
	return new GHNullMutex();
}

GHThreadSleeper* GHNullThreadFactory::createSleeper(void) const
{
	return new GHNullThreadSleeper();
}

GHThreadSignal* GHNullThreadFactory::createSignal(void) const
{
	return new GHNullThreadSignal();
}

