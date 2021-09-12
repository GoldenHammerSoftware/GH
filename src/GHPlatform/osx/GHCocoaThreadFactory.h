// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHThreadFactory.h"

class GHCocoaThreadFactory : public GHThreadFactory
{
public:
	virtual GHThread* createThread(GHThread::Priority pri) const override;
	virtual GHMutex* createMutex(void) const override;
	virtual GHThreadSleeper* createSleeper(void) const override;
    virtual GHThreadSignal* createSignal(void) const override;
};
