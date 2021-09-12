// Copyright 2010 Golden Hammer Software
#include "GHAndroidThread.h"
#include "GHPlatform/GHRunnable.h"
#include <pthread.h>
#include "GHPlatform/GHDebugMessage.h"
#include "GHJNIMgr.h"

GHAndroidThread::GHAndroidThread(GHJNIMgr& jniMgr)
: mJNIMgr(jniMgr)
{
}

struct ThreadArgs
{
	GHRunnable* mRunnable;
	JavaVM* mJVM;
};

void* threadLaunch(void* arg)
{
	ThreadArgs* threadArgs = (ThreadArgs*)arg;
	JNIEnv* jniEnv;
	bool attachNeeded = true;

	int status = threadArgs->mJVM->GetEnv((void**)&jniEnv, JNI_VERSION_1_6);
	if (status == JNI_OK)
	{
		attachNeeded = false;
	}

	if (attachNeeded)
	{
		threadArgs->mJVM->AttachCurrentThread(&jniEnv, NULL);
	}

	threadArgs->mRunnable->run();

	if (attachNeeded)
	{
		threadArgs->mJVM->DetachCurrentThread();
	}

	delete threadArgs;

	return 0;
}

void GHAndroidThread::runThread(GHRunnable& runnable) 
{ 
	GHDebugMessage::outputString("GHAndroidThread::runThread");

	ThreadArgs* threadArgs = new ThreadArgs;
	threadArgs->mRunnable = &runnable;
	threadArgs->mJVM = mJNIMgr.getJVM();

	pthread_t myPThread;
	if (pthread_create(&myPThread, NULL, threadLaunch, (void*)(threadArgs)))
	{
		GHDebugMessage::outputString("Error creating pthread");
		return;
	}
	pthread_setname_np(myPThread, "GHThread");
}
