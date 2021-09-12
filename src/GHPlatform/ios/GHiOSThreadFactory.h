// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHThreadFactory.h"

class GHiOSThreadFactory : public GHThreadFactory
{
public:
    virtual GHThread* createThread(GHThread::Priority pri) const;
	virtual GHMutex* createMutex(void) const;
	virtual GHThreadSleeper* createSleeper(void) const;
    virtual GHThreadSignal* createSignal(void) const;
};
