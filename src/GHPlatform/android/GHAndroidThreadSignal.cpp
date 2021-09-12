// Copyright Golden Hammer Software
#include "GHAndroidThreadSignal.h"
#include "GHAndroidMutex.h"
#include "GHPlatform/GHDebugMessage.h"

GHAndroidThreadSignal::GHAndroidThreadSignal(void)
{
	int success = pthread_cond_init(&mCondition, 0);
	if (success != 0)
	{
		GHDebugMessage::outputString("Failed to initialize signal");
	}
}

GHAndroidThreadSignal::~GHAndroidThreadSignal(void)
{
	pthread_cond_destroy(&mCondition);
}

bool GHAndroidThreadSignal::wait(GHMutex& mutex)
{
	// assumming it's an android mutex
	GHAndroidMutex* androidMutex = (GHAndroidMutex*)&mutex;
	pthread_mutex_t& pthreadMutex = androidMutex->getPThreadMutex();

	int success = pthread_cond_wait(&mCondition, &pthreadMutex);
	if (success != 0)
	{
		GHDebugMessage::outputString("Failed to wait for signal");
		return false;
	}
	return true;
}

bool GHAndroidThreadSignal::signal(void)
{
	int success = pthread_cond_broadcast(&mCondition);
	if (success != 0)
	{
		GHDebugMessage::outputString("Failed to broadcast signal");
		return false;
	}
	return true;
}