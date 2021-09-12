// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHThreadSignal.h"
#include <pthread.h>

class GHAndroidThreadSignal : public GHThreadSignal
{
public:
	GHAndroidThreadSignal(void);
	~GHAndroidThreadSignal(void);

	virtual bool wait(GHMutex& mutex);
	virtual bool signal(void);

private:
	pthread_cond_t mCondition;
};
