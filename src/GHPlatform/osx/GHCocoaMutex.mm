// Copyright 2010 Golden Hammer Software
#include "GHCocoaMutex.h"

GHCocoaMutex::GHCocoaMutex(void)
{
	mLock = [[NSRecursiveLock alloc] init];
}

GHCocoaMutex::~GHCocoaMutex(void)
{
}

bool GHCocoaMutex::acquire(void)
{
	[mLock lock];
	return true;
}

bool GHCocoaMutex::release(void)
{
	[mLock unlock];
	return true;
}
