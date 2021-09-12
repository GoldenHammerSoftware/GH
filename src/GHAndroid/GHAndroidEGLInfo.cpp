#include "GHAndroidEGLInfo.h"

GHAndroidEGLInfo* GHAndroidEGLInfo::sEGLInfo = 0;

GHAndroidEGLInfo* GHAndroidEGLInfo::get(void)
{
	if (!sEGLInfo)
	{
		sEGLInfo = new GHAndroidEGLInfo;
	}
	return sEGLInfo;
}

void GHAndroidEGLInfo::clearEGL(void)
{
	mEGLDisplay = EGL_NO_DISPLAY;
	mEGLSurface = EGL_NO_SURFACE;
	mEGLContext = EGL_NO_CONTEXT;
}
