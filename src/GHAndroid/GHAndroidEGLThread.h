#pragma once

#include "GHPlatform/GHRunnable.h"
#include <EGL/egl.h> 
#include <GLES/gl.h>

class ANativeWindow;
class GHAndroidAppRunner;
class GHThreadFactory;
class GHThread;
class GHMutex;

// manages initializing egl and running a thread for it.
class GHAndroidEGLThread : public GHRunnable
{
public:
	GHAndroidEGLThread(const GHThreadFactory& threadFactory, GHAndroidAppRunner& runner);
	~GHAndroidEGLThread(void);

	void resume(void);
	void pause(void);

	virtual void run(void);

	void setWindow(ANativeWindow* window);

private:
	bool initEGL(void);
	void shutdownEGL(void);
	bool chooseEGLConfig(EGLDisplay& display, EGLConfig& config);

private:
	const GHThreadFactory& mThreadFactory;
	GHAndroidAppRunner& mRunner;
	GHThread* mThread{ 0 };
	GHMutex* mMutex{ 0 }; // mutex to wait for one thread iteration.
	GHMutex* mRunningMutex{ 0 }; // mutex to wait for thread completion.
	GHMutex* mPauseMutex{ 0 };

	bool mAborted{ false };
	int mNumTicksSinceEGLCreate{ 0 };
};

