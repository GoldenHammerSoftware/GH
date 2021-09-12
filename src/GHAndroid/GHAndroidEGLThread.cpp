#include "GHAndroidEGLThread.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHThread.h"
#include "GHPlatform/GHMutex.h"
#include "GHAndroidAppRunner.h"
#include "GHPlatform/GHDebugMessage.h"
#include <android/native_window.h>
#include "GHAndroidEGLInfo.h"
#include "GHUtils/GHMessage.h"
#include "GHAndroidIdentifiers.h"

#if !defined( EGL_OPENGL_ES3_BIT_KHR )
#define EGL_OPENGL_ES3_BIT_KHR		0x0040
#endif

// todo: replace the GH_OVR_GO ifdefs with an interface we pass in.

GHAndroidEGLThread::GHAndroidEGLThread(const GHThreadFactory& threadFactory, GHAndroidAppRunner& runner)
	: mThreadFactory(threadFactory)
	, mRunner(runner)
{
	mMutex = mThreadFactory.createMutex();
	mRunningMutex = mThreadFactory.createMutex();
	mPauseMutex = mThreadFactory.createMutex();
	mThread = mThreadFactory.createThread(GHThread::TP_DEFAULT);

	// start the thread paused.
	mPauseMutex->acquire();
	mAborted = false;
	mThread->runThread(*this);
}

GHAndroidEGLThread::~GHAndroidEGLThread(void)
{
	mPauseMutex->release();
	mMutex->acquire();
	mAborted = true;
	mMutex->release();

	// wait for completion
	mRunningMutex->acquire();
	mRunningMutex->release();

	delete mRunningMutex;
	delete mMutex;
	delete mThread;
}

void GHAndroidEGLThread::resume(void)
{
	mPauseMutex->release();
}

void GHAndroidEGLThread::pause(void)
{
	mPauseMutex->acquire();
}

void GHAndroidEGLThread::run(void)
{
	mRunningMutex->acquire();

	while (!mAborted)
	{
		// grab a mutex for pausing this thread.
		mPauseMutex->acquire();
		mPauseMutex->release();

		mMutex->acquire();

        if (GHAndroidEGLInfo::get()->getWindow() == 0 && GHAndroidEGLInfo::get()->getDisplay() != EGL_NO_DISPLAY)
        {
            shutdownEGL();
        }
		if (GHAndroidEGLInfo::get()->getWindow() != 0 && !GHAndroidEGLInfo::get()->getDisplay())
		{
			initEGL();
		}
		if (!GHAndroidEGLInfo::get()->getDisplay())
		{
			// not ready yet, wait for incoming window.
			mMutex->release();
			continue;
		}

		// we seem to be getting an init and then immediate stop if the headset isn't worn when launching.
		// maybe this will help.
		if (mNumTicksSinceEGLCreate > 10)
		{
			mRunner.onTick();
		}
		else
		{
			mNumTicksSinceEGLCreate++;
		}

#ifndef GH_OVR_GO
		// ovrgo handles the swap buffers in a timewarp thread.

		if (!eglSwapBuffers(GHAndroidEGLInfo::get()->getDisplay(), GHAndroidEGLInfo::get()->getSurface()))
		{
			GHDebugMessage::outputString("eglSwapBuffers() returned error %d", eglGetError());
		}
#endif

		mMutex->release();
	} 
	// pump the message queue one more time.
	// ensures that stuff like onpause was hit.
	mRunner.sendAppMessages();

	shutdownEGL();

	mRunningMutex->release();
}

void GHAndroidEGLThread::setWindow(ANativeWindow* window)
{
	GHAndroidEGLInfo::get()->setWindow(window);

	/*
	mMutex->acquire();
	ANativeWindow* prevWindow = GHAndroidEGLInfo::get()->getWindow();
	if (prevWindow && prevWindow != window)
	{
		GHDebugMessage::outputString("Releasing ANativeWindow");
		// if we called ANativeWindow_acquire here then we need to call ANativeWindow_release
		// otherwise we shouldn't have to.
		// as of this writing, acquire happens in GHAndroidMain ANativeWindow_fromSurface,
		//  and GHAndroidAppRunner handles the release.
		//ANativeWindow_release(prevWindow);
	}
	GHAndroidEGLInfo::get()->setWindow(window);
	if (!window)
	{
		GHDebugMessage::outputString("AndroidEGL setWindow 0");
	}
	mMutex->release();
	*/
}

bool GHAndroidEGLThread::initEGL(void)
{
	//https://github.com/tsaarni/android-native-egl-example/blob/master/src/main/jni/renderer.cpp

	EGLDisplay display;
	EGLConfig config;
	EGLint format;
	EGLSurface surface;
	EGLContext context;
	GLfloat ratio;

	GHDebugMessage::outputString("Initializing EGL context");

	if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
		GHDebugMessage::outputString("eglGetDisplay() returned error %d", eglGetError());
		return false;
	}
	if (!eglInitialize(display, 0, 0)) {
		GHDebugMessage::outputString("eglInitialize() returned error %d", eglGetError());
		return false;
	}

	if (!chooseEGLConfig(display, config))
	{
		GHDebugMessage::outputString("eglChooseConfig() returned error %d", eglGetError());
		shutdownEGL();
		return false;
	}

	if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
		GHDebugMessage::outputString("eglGetConfigAttrib() returned error %d", eglGetError());
		shutdownEGL();
		return false;
	}

#ifndef GH_OVR_GO
	ANativeWindow_setBuffersGeometry(GHAndroidEGLInfo::get()->getWindow(), 0, 0, format);

	if (!(surface = eglCreateWindowSurface(display, config, GHAndroidEGLInfo::get()->getWindow(), 0))) {
		GHDebugMessage::outputString("eglCreateWindowSurface() returned error %d", eglGetError());
		shutdownEGL();
		return false;
	}
#else
	const EGLint surfaceAttribs[] =
	{
		EGL_WIDTH, 16,
		EGL_HEIGHT, 16,
		EGL_NONE
	};
	GHDebugMessage::outputString("surface = eglCreatePbufferSurface( Display, Config, surfaceAttribs )");
	surface = eglCreatePbufferSurface(display, config, surfaceAttribs);
	if (surface == EGL_NO_SURFACE)
	{
		GHDebugMessage::outputString("Failed to create tiny surface for ovr");
		shutdownEGL();
		return false;
	}
#endif

	// try to initialize es3, if that fails initialize es2
	const EGLint EGL3ContextAttribList[] = {
		EGL_CONTEXT_CLIENT_VERSION, 3,
		EGL_NONE
	};
	const EGLint EGL2ContextAttribList[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	if (!(context = eglCreateContext(display, config, EGL_NO_CONTEXT, EGL3ContextAttribList))) 
	{
		if (!(context = eglCreateContext(display, config, EGL_NO_CONTEXT, EGL2ContextAttribList)))
		{
			GHDebugMessage::outputString("eglCreateContext() returned error %d", eglGetError());
			shutdownEGL();
			return false;
		}
	}

	if (!eglMakeCurrent(display, surface, surface, context)) {
		GHDebugMessage::outputString("eglMakeCurrent() returned error %d", eglGetError());
		shutdownEGL();
		return false;
	}

	GHAndroidEGLInfo::get()->setDisplay(display);
	GHAndroidEGLInfo::get()->setSurface(surface);
	GHAndroidEGLInfo::get()->setContext(context);
	GHDebugMessage::outputString("Created EGL");

	mNumTicksSinceEGLCreate = 0;

	return true;
}

bool GHAndroidEGLThread::chooseEGLConfig(EGLDisplay& display, EGLConfig& config)
{
#ifndef GH_OVR_GO
	EGLint numConfigs;

	// try es3 first, if that doesn't work then fallback to es2
	const EGLint attribs_es3[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_NONE
	};

	if (!eglChooseConfig(display, attribs_es3, &config, 1, &numConfigs)) {
		GHDebugMessage::outputString("Failed to find es3 config, error %d", eglGetError());
	}
	else
	{
		EGLint format;
		if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
			GHDebugMessage::outputString("ES3 config doesn't look good, falling back to ES2");
		}
		else
		{
			GHDebugMessage::outputString("Found ES3 config");
			return true;
		}
	}

	// fallback to es2
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_NONE
	};

	if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
		GHDebugMessage::outputString("eglChooseConfig() returned error %d", eglGetError());
		return false;
	}
	GHDebugMessage::outputString("Found ES2 config");
	return true;

#else
	// ovrgo wants different egl setup.

	// Do NOT use eglChooseConfig, because the Android EGL code pushes in multisample
	// flags in eglChooseConfig if the user has selected the "force 4x MSAA" option in
	// settings, and that is completely wasted for our warp target.
	const int MAX_CONFIGS = 1024;
	EGLConfig configs[MAX_CONFIGS];
	EGLint numConfigs = 0;
	if (eglGetConfigs(display, configs, MAX_CONFIGS, &numConfigs) == EGL_FALSE)
	{
		GHDebugMessage::outputString("eglGetConfigs() failed: %d", eglGetError());
		return false;
	}
	const EGLint configAttribs[] =
	{
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE,		8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE,		8, // need alpha for the multi-pass timewarp compositor
		EGL_DEPTH_SIZE,		0,
		EGL_STENCIL_SIZE,	0,
		EGL_SAMPLES,		0,
		EGL_NONE
	};
	EGLConfig* foundConfig = 0;
	for (int i = 0; i < numConfigs; i++)
	{
		EGLint value = 0;

		eglGetConfigAttrib(display, configs[i], EGL_RENDERABLE_TYPE, &value);
		if ((value & EGL_OPENGL_ES3_BIT_KHR) != EGL_OPENGL_ES3_BIT_KHR)
		{
			continue;
		}

		// The pbuffer config also needs to be compatible with normal window rendering
		// so it can share textures with the window context.
		eglGetConfigAttrib(display, configs[i], EGL_SURFACE_TYPE, &value);
		if ((value & (EGL_WINDOW_BIT | EGL_PBUFFER_BIT)) != (EGL_WINDOW_BIT | EGL_PBUFFER_BIT))
		{
			continue;
		}

		int	j = 0;
		for (; configAttribs[j] != EGL_NONE; j += 2)
		{
			eglGetConfigAttrib(display, configs[i], configAttribs[j], &value);
			if (value != configAttribs[j + 1])
			{
				break;
			}
		}
		if (configAttribs[j] == EGL_NONE)
		{
			foundConfig = &configs[i];
			break;
		}
	}
	if (foundConfig == 0)
	{
		GHDebugMessage::outputString("eglChooseConfig() failed: %s", eglGetError());
		return false;
	}

	config = *foundConfig;
	return true;

#endif
}

void GHAndroidEGLThread::shutdownEGL(void)
{
	GHDebugMessage::outputString("Destroying EGL context");

	if (eglMakeCurrent(GHAndroidEGLInfo::get()->getDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE)
	{
		GHDebugMessage::outputString("shutdownEGL - failed to make current");
	}
	if (eglDestroyContext(GHAndroidEGLInfo::get()->getDisplay(), GHAndroidEGLInfo::get()->getContext()) == EGL_FALSE)
	{
		GHDebugMessage::outputString("shutdownEGL - failed to destroy context");
	}
	if (eglDestroySurface(GHAndroidEGLInfo::get()->getDisplay(), GHAndroidEGLInfo::get()->getSurface()) == EGL_FALSE)
	{
		GHDebugMessage::outputString("shutdownEGL - failed to destroy surface");
	}
	if (eglTerminate(GHAndroidEGLInfo::get()->getDisplay()) == EGL_FALSE)
	{
		GHDebugMessage::outputString("shutdownEGL - failed to eglTerminate");
	}

	GHAndroidEGLInfo::get()->clearEGL();
	GHDebugMessage::outputString("Shutdown EGL");
}
