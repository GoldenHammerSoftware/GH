// Copyright Golden Hammer Software
#pragma once

#include "GHWin32Include.h"
#include "GHPlatform/GHMutex.h"

//#define GH_WIN32_USE_CRITICAL_SECTION

class GHWin32Mutex : public GHMutex
{
public:
	GHWin32Mutex(void);
	~GHWin32Mutex(void);

	virtual bool acquire(void);
	virtual bool release(void);

private:

#ifdef GH_WIN32_USE_CRITICAL_SECTION
	// ok, so we're using a critical section and not a mutex...
	CRITICAL_SECTION mCriticalSection;
#else
	HANDLE mMutex;
#endif
};
