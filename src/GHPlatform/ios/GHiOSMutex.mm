// Copyright 2010 Golden Hammer Software
#include "GHiOSMutex.h"
#include "GHDebugMessage.h"

GHiOSMutex::GHiOSMutex(void)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    int ret = pthread_mutex_init(&mMutex, &attr);
    if (ret != 0) {
        GHDebugMessage::outputString("Failed to create mutex!!! %d", ret);
    }
}

GHiOSMutex::~GHiOSMutex(void)
{
    pthread_mutex_destroy(&mMutex);
}

bool GHiOSMutex::acquire(void)
{
    int ret = pthread_mutex_lock(&mMutex);
    if (ret != 0) {
        GHDebugMessage::outputString("Failed to lock mutex!!! %d", ret);
    }
    return true;
}

bool GHiOSMutex::release(void)
{
    pthread_mutex_unlock(&mMutex);
	return true;
}
