// Copyright Golden Hammer Software
#include "GHAndroidEGLFence.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHAndroidEGLInfo.h"
#include "GHGLESErrorReporter.h"

PFNEGLCREATESYNCKHRPROC			eglCreateSyncKHR;
PFNEGLDESTROYSYNCKHRPROC		eglDestroySyncKHR;
PFNEGLCLIENTWAITSYNCKHRPROC		eglClientWaitSyncKHR;
PFNEGLSIGNALSYNCKHRPROC			eglSignalSyncKHR;
PFNEGLGETSYNCATTRIBKHRPROC		eglGetSyncAttribKHR;

GHAndroidEGLFence::GHAndroidEGLFence()
{
	static bool eglInited = false;
	if (!eglInited)
	{
		eglInited = true;

		eglCreateSyncKHR = (PFNEGLCREATESYNCKHRPROC)eglGetProcAddress("eglCreateSyncKHR");
		eglDestroySyncKHR = (PFNEGLDESTROYSYNCKHRPROC)eglGetProcAddress("eglDestroySyncKHR");
		eglClientWaitSyncKHR = (PFNEGLCLIENTWAITSYNCKHRPROC)eglGetProcAddress("eglClientWaitSyncKHR");
		eglSignalSyncKHR = (PFNEGLSIGNALSYNCKHRPROC)eglGetProcAddress("eglSignalSyncKHR");
		eglGetSyncAttribKHR = (PFNEGLGETSYNCATTRIBKHRPROC)eglGetProcAddress("eglGetSyncAttribKHR");
	}
}

GHAndroidEGLFence::~GHAndroidEGLFence(void)
{
	destroyFence();
}

void GHAndroidEGLFence::createFence(void)
{
	destroyFence();
	mEGLSync = eglCreateSyncKHR(GHAndroidEGLInfo::get()->getDisplay(), EGL_SYNC_FENCE_KHR, NULL);
	if (mEGLSync == EGL_NO_SYNC_KHR)
	{
		GHDebugMessage::outputString("failed to eglCreateSyncKHR()");
		return;
	}
}

void GHAndroidEGLFence::destroyFence(void)
{
	if (mEGLSync != EGL_NO_SYNC_KHR)
	{
		waitForFence();
		if (eglDestroySyncKHR(GHAndroidEGLInfo::get()->getDisplay(), mEGLSync) == EGL_FALSE)
		{
			GHDebugMessage::outputString("Failed to destroy egl fence!");
		}
		mEGLSync = EGL_NO_SYNC_KHR;
	}
}

void GHAndroidEGLFence::waitForFence(void)
{
	// Force flushing the commands.
	// Note that some drivers will already flush when calling eglCreateSyncKHR.
	if (eglClientWaitSyncKHR(GHAndroidEGLInfo::get()->getDisplay(), mEGLSync, EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, 0) == EGL_FALSE)
	{
		GHDebugMessage::outputString("failed to eglClientWaitSyncKHR()");
		return;
	}
}
