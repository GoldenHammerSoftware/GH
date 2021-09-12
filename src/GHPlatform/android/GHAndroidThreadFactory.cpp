// Copyright 2010 Golden Hammer Software
#include "GHAndroidThreadFactory.h"
#include "GHAndroidThread.h"
#include "GHAndroidMutex.h"
#include "GHAndroidThreadSleeper.h"
#include "GHAndroidThreadSignal.h"

GHAndroidThreadFactory::GHAndroidThreadFactory(GHJNIMgr& jniMgr)
: mJNIMgr(jniMgr)
{
}

GHThread* GHAndroidThreadFactory::createThread(GHThread::Priority pri) const
{
	return new GHAndroidThread(mJNIMgr);
}

GHMutex* GHAndroidThreadFactory::createMutex(void) const
{
	return new GHAndroidMutex;
}

GHThreadSleeper* GHAndroidThreadFactory::createSleeper(void) const
{
	return new GHAndroidThreadSleeper;
}

GHThreadSignal* GHAndroidThreadFactory::createSignal(void) const
{
	return new GHAndroidThreadSignal;
}