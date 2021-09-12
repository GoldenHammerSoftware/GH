// Copyright 2010 Golden Hammer Software
#include "GHMetroThreadFactory.h"
#include "GHMetroThread.h"
#include "GHMetroMutex.h"
#include "GHMetroThreadSleeper.h"
#include "GHPlatform/null/GHNullThreadSignal.h"

GHThread* GHMetroThreadFactory::createThread(GHThread::Priority pri) const
{
	// todo: support priority.
	return new GHMetroThread;
}

GHMutex* GHMetroThreadFactory::createMutex(void) const
{
	return new GHMetroMutex;
}

GHThreadSleeper* GHMetroThreadFactory::createSleeper(void) const
{
	return new GHMetroThreadSleeper;
}

GHThreadSignal* GHMetroThreadFactory::createSignal(void) const
{
	// todo
	return new GHNullThreadSignal;
}