// Copyright 2010 Golden Hammer Software
#include "GHMetroMutex.h"
#include "GHPlatform/GHDebugMessage.h"

#ifdef GH_WINRT_USE_CRITICAL_SECTION

GHMetroMutex::GHMetroMutex(void)
{
	// the third argument could be CRITICAL_SECTION_NO_DEBUG_INFO
	InitializeCriticalSectionEx(&mCriticalSection, 0, 0);
}

GHMetroMutex::~GHMetroMutex(void)
{
	DeleteCriticalSection(&mCriticalSection);
}

bool GHMetroMutex::acquire(void)
{
	EnterCriticalSection(&mCriticalSection);
	return true;
}

bool GHMetroMutex::release(void)
{
	LeaveCriticalSection(&mCriticalSection);
	return true;
}

#else

GHMetroMutex::GHMetroMutex(void)
{
	mMutex = CreateMutexEx(NULL, NULL, 0, SYNCHRONIZE);
	if (!mMutex)
	{
		GHDebugMessage::outputString("Failed to create mutex");
	}
}

GHMetroMutex::~GHMetroMutex(void)
{
	CloseHandle(mMutex);
}

bool GHMetroMutex::acquire(void)
{
	DWORD dwWaitResult = WaitForSingleObjectEx(mMutex, INFINITE, FALSE);
	if (dwWaitResult != WAIT_OBJECT_0)
	{
		DWORD err = GetLastError();
		GHDebugMessage::outputString("Failed to acquire mutex %d", err);
		return false;
	}
	return true;
}

bool GHMetroMutex::release(void)
{
	if (!ReleaseMutex(mMutex))
	{
		GHDebugMessage::outputString("Failed to release mutex");
		return false;
	}
	return true;
}

#endif