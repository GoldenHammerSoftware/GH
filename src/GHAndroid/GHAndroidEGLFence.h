// Copyright Golden Hammer Software
#pragma once

#include <EGL/egl.h> 
#include <EGL/eglext.h> 

class GHAndroidEGLInfo;

// Wrapper for EGLSyncKHR
// A fence is a way to push a flag to the gpu and then have a cpu thread wait for it to complete.
class GHAndroidEGLFence
{
public:
	GHAndroidEGLFence(void);
	~GHAndroidEGLFence(void);

	void createFence(void);
	void destroyFence(void);
	void waitForFence(void);

	EGLSyncKHR getFenceHandle(void) { return mEGLSync; }

private:
	EGLSyncKHR mEGLSync{ EGL_NO_SYNC_KHR };
};
