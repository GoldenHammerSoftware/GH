// Copyright Golden Hammer Software
#include "GHWin32Mutex.h"
#include "GHPlatform/GHDebugMessage.h"

#ifdef GH_WIN32_USE_CRITICAL_SECTION

GHWin32Mutex::GHWin32Mutex(void)
{
	InitializeCriticalSection(&mCriticalSection);
}

GHWin32Mutex::~GHWin32Mutex(void)
{
	DeleteCriticalSection(&mCriticalSection);
}

bool GHWin32Mutex::acquire(void)
{
	EnterCriticalSection(&mCriticalSection);
	return true;
}

bool GHWin32Mutex::release(void)
{
	LeaveCriticalSection(&mCriticalSection);
	return true;
}

#else

GHWin32Mutex::GHWin32Mutex(void)
{
	mMutex = CreateMutex(NULL, FALSE, NULL);
	if (!mMutex)
	{
		GHDebugMessage::outputString("Failed to create mutex");
	}
}

GHWin32Mutex::~GHWin32Mutex(void)
{
	CloseHandle(mMutex);
}

bool GHWin32Mutex::acquire(void)
{
	DWORD dwWaitResult = WaitForSingleObject(mMutex, INFINITE); 
	if (dwWaitResult != WAIT_OBJECT_0)
	{
		GHDebugMessage::outputString("Failed to acquire mutex");
		return false;
	}
	return true;
}

bool GHWin32Mutex::release(void)
{
	if (!ReleaseMutex(mMutex))
	{
		GHDebugMessage::outputString("Failed to release mutex");
		return false;
	}
	return true;
}

#endif