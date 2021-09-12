#pragma once

#include <EGL/egl.h> 
#include <GLES/gl.h>
#include <atomic>
class ANativeWindow;

// singleton for holding info about the current egl session.
class GHAndroidEGLInfo
{
public:
	static GHAndroidEGLInfo* get(void);

	void clearEGL(void);

	ANativeWindow* getWindow(void) { return mWindow; }
	void setWindow(ANativeWindow* window) { mWindow = window; }
	const EGLDisplay& getDisplay(void) const { return mEGLDisplay; }
	void setDisplay(EGLDisplay& display) { mEGLDisplay = display; }
	const EGLSurface& getSurface(void) const { return mEGLSurface; }
	void setSurface(EGLSurface& surface) { mEGLSurface = surface; }
	const EGLContext& getContext(void) const { return mEGLContext; }
	void setContext(EGLContext& context) { mEGLContext = context; }

private:
	static GHAndroidEGLInfo* sEGLInfo;

	std::atomic<ANativeWindow*> mWindow{ 0 };
	EGLDisplay mEGLDisplay{ EGL_NO_DISPLAY };
	EGLSurface mEGLSurface{ EGL_NO_SURFACE };
	EGLContext mEGLContext{ EGL_NO_CONTEXT };
};