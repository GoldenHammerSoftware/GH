// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMutex.h"
#include <pthread.h>

class GHiOSMutex : public GHMutex
{
public:
	GHiOSMutex(void);
	~GHiOSMutex(void);
	
	virtual bool acquire(void);
	virtual bool release(void);
    
private:
    pthread_mutex_t mMutex;
};
