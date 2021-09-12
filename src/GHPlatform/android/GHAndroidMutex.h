// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHMutex.h"
#include <pthread.h>

class GHAndroidMutex : public GHMutex
{
public:
    GHAndroidMutex(void);
    ~GHAndroidMutex(void);
    
	virtual bool acquire(void);
	virtual bool release(void);
    
	// need access to the pthread mutex in signal.
	// a bit ugly.
	pthread_mutex_t& getPThreadMutex(void) { return mMutex; }

private:
    pthread_mutex_t mMutex;
};