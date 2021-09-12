// Copyright Golden Hammer Software
#include "GHWin32Thread.h"
#include "GHPlatform/GHRunnable.h"
#include "GHPlatform/GHMutex.h"
#include "GHPlatform/GHDebugMessage.h"

// Have to fake out the win32 C api a little with a static function.
DWORD WINAPI runWin32Thread(LPVOID param)
{
	GHWin32Thread::Win32ThreadArgs* threadArgs = ((GHWin32Thread::Win32ThreadArgs*)param);
	threadArgs->runnable->run();
	return 0;
}

GHWin32Thread::GHWin32Thread(void)
: mWin32ThreadHandle(0)
{
}

GHWin32Thread::~GHWin32Thread(void)
{
	if (mWin32ThreadHandle) {
		CloseHandle(mWin32ThreadHandle);
		mWin32ThreadHandle = 0;
	}
}

void GHWin32Thread::runThread(GHRunnable& runnable)
{
	// close previous thread handle if we still have one.
	if (mWin32ThreadHandle) {
		CloseHandle(mWin32ThreadHandle);
		mWin32ThreadHandle = 0;
	}

	DWORD creationFlags = 0;
	mThreadArgs.runnable = &runnable;

	mWin32ThreadHandle = CreateThread(NULL, 0, runWin32Thread, &mThreadArgs, 0, &creationFlags);
	if (!mWin32ThreadHandle) {
		GHDebugMessage::outputString("Failed to create thread.\n");
	}
}
