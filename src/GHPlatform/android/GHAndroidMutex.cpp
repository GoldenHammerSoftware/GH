// Copyright 2010 Golden Hammer Software
#include "GHAndroidMutex.h"
#include "GHPlatform/GHDebugMessage.h"

GHAndroidMutex::GHAndroidMutex(void)
{
    pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&mMutex, &attr);
}

GHAndroidMutex::~GHAndroidMutex(void)
{
    pthread_mutex_destroy(&mMutex);
}

bool GHAndroidMutex::acquire(void)
{
    int error = pthread_mutex_lock(&mMutex);
    if (error != 0)
    {
        GHDebugMessage::outputString("GHAndroidMutex::lock() error");
    }
	return error == 0;
}

bool GHAndroidMutex::release(void)
{
	int error = pthread_mutex_unlock(&mMutex);
	if (error != 0)
	{
	    GHDebugMessage::outputString("GHAndroidMutex::unlock() error");
	}
	return error == 0;
}
