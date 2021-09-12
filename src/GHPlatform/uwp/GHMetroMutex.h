// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHMutex.h"
#include <windows.h>

//#define GH_WINRT_USE_CRITICAL_SECTION

class GHMetroMutex : public GHMutex
{
public:
	GHMetroMutex(void);
	~GHMetroMutex(void);

	virtual bool acquire(void);
	virtual bool release(void);

private:
#ifdef GH_WINRT_USE_CRITICAL_SECTION
	// ok, so we're using a critical section and not a mutex...
	CRITICAL_SECTION mCriticalSection;
#else
	HANDLE mMutex;
#endif
};
