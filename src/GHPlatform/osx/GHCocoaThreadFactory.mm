// Copyright 2010 Golden Hammer Software
#include "GHCocoaThreadFactory.h"
#include "GHCocoaThread.h"
#include "GHCocoaMutex.h"
#include "GHCocoaThreadSleeper.h"
#include "GHNullThreadSignal.h"

GHThread* GHCocoaThreadFactory::createThread(GHThread::Priority pri) const
{
	return new GHCocoaThread;
}

GHMutex* GHCocoaThreadFactory::createMutex(void) const
{
	return new GHCocoaMutex;
}

GHThreadSleeper* GHCocoaThreadFactory::createSleeper(void) const
{
	return new GHCocoaThreadSleeper;
}

GHThreadSignal* GHCocoaThreadFactory::createSignal(void) const
{
    return new GHNullThreadSignal;
}
