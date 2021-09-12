// Copyright Golden Hammer Software
#pragma once

#include "GHWin32Include.h"
#include "GHPlatform/GHThread.h"

class GHMutex;

class GHWin32Thread : public GHThread
{
public:
	GHWin32Thread(void);
	~GHWin32Thread(void);

	virtual void runThread(GHRunnable& runnable);

public:
	struct Win32ThreadArgs
	{
		GHRunnable* runnable;
		GHMutex* mutex;
	};

private:
	HANDLE mWin32ThreadHandle;
	Win32ThreadArgs mThreadArgs;
};
