#include "GHAndroidAppRunner.h"
#include <android/log.h>
#include <android/native_window.h>

#include "GHAndroidSystemServices.h"
#include "GHInputState.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHAndroidRenderServices.h"
#include "GHAndroidGameServices.h"
#include "GHIAPStoreFactory.h"
#include "GHStatTrackerFactory.h"
#include "GHMultiplayerFactory.h"
#include "GHAdFactory.h"
#include "GHPlatform/GHMutex.h"
#include "GHAppRunner.h"
#include "GHApp.h"
#include "GHPlatform/android/GHJNIMgr.h"
#include "GHAndroidEGLThread.h"

#include "GHGLESErrorReporter.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHKeyDef.h"
#include "GHMath/GHPoint.h"
#include "GHAndroidIAPStoreFactory.h"
#include "GHNullIAPStoreFactory.h"
#include "GHStatTrackerFactoryLocal.h"
#include "GHNullMultiplayerFactory.h"
#include "GHAndroidAdFactory.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"

GHAndroidAppRunner::GHAndroidAppRunner(JNIEnv* env, JavaVM* jvm, jobject javaEngineInterface, jobject jAssetMgr, jobject activity, std::vector<GHString>&& saveFileDirectories)
	: mJavaEngineInterface(javaEngineInterface)
{
	if (!jvm)
	{
		GHDebugMessage::outputString("JVM not cached, JNI_OnLoad not called?");
	}
	mJNIMgr = new GHJNIMgr(*env, jvm, activity);

	mSystemServices = new GHAndroidSystemServices(*mJNIMgr, mJavaEngineInterface, std::move(saveFileDirectories), jAssetMgr);
}

GHAndroidAppRunner::~GHAndroidAppRunner(void)
{
	GHGLESErrorReporter::setErrorReportingEnabled(false);
	delete mAppRunner;
	delete mApp; 
	delete mInputMutex;
	delete mAdFactory;
	delete mMultiplayerFactory;
	delete mStatTrackerFactory;
	delete mIAPStoreFactory;
	delete mGameServices;
	delete mRenderServices;
	delete mAppMessageQueue;
	delete mVolatileInputState;
	delete mSystemServices;
	delete mJNIMgr;

    if (mWindow) ANativeWindow_release(mWindow);
}

void GHAndroidAppRunner::launchNativeApp(int windowWidth, int windowHeight)
{
	GHDebugMessage::outputString("Android GH started.");

	mVolatileInputState = new GHInputState(10, 1, 0, 3, 2, 2, 1);
	mAppMessageQueue = new GHMessageQueue(&mSystemServices->getPlatformServices().getThreadFactory());
	mRenderServices = new GHAndroidRenderServices(windowWidth, windowHeight, *mSystemServices, mSystemServices->getPlatformServices().getIdFactory(), *mJNIMgr, 
		mJavaEngineInterface, mSystemServices->getPlatformServices().getFileOpener());
	mGameServices = new GHAndroidGameServices(*mJNIMgr, mJavaEngineInterface, *mSystemServices, *mRenderServices, *mAppMessageQueue);

	if (mUseIAP) {
		mIAPStoreFactory = new GHAndroidIAPStoreFactory(*mJNIMgr, mJavaEngineInterface, mSystemServices->getEventMgr(), *mAppMessageQueue);
	}
	else {
		mIAPStoreFactory = new GHNullIAPStoreFactory(true);
	}
	mStatTrackerFactory = new GHStatTrackerFactoryLocal(mSystemServices->getXMLSerializer());
	mMultiplayerFactory = new GHNullMultiplayerFactory;
	mAdFactory = new GHAndroidAdFactory(*mJNIMgr, mJavaEngineInterface, mSystemControllerMgr, *mAppMessageQueue);
	mInputMutex = mSystemServices->getPlatformServices().getThreadFactory().createMutex();

	createApp();

	GHDebugMessage::outputString("About to create App runner");
	mAppRunner = new GHAppRunner(mApp,
		mSystemServices->getEventMgr(),
		*mAppMessageQueue,
		mSystemServices->getInputState(),
		*mVolatileInputState,
		*mInputMutex,
		mSystemServices->getPlatformServices().getThreadFactory(),
		0);
	GHDebugMessage::outputString("Created App Runner");
}

void GHAndroidAppRunner::onTick(void)
{
	if (!mAppRunner) {
		launchNativeApp(mWindowWidth, mWindowHeight);
	}
	mAppRunner->runFrame();
	mSystemControllerMgr.update();
}

void GHAndroidAppRunner::sendAppMessages(void)
{
	if (!mAppRunner) return;
	mAppRunner->sendAppMessages();
}

void GHAndroidAppRunner::handleImmediateAppMessage(const GHMessage& message)
{
	// we throw away any messages from before the app starts.
	if (!mAppRunner) return;
	mAppRunner->handleImmediateAppMessage(message);;
}

void GHAndroidAppRunner::onResize(int width, int height)
{
	if (mRenderServices)
	{
		mRenderServices->getScreenInfo().setSize(GHPoint2i(width, height));
		mRenderServices->getScreenInfo().setPixelSizeFactor(1 / 800.f, 0.0f);
	}

	if (mAppMessageQueue)
	{
		GHDebugMessage::outputString("sending WINDOWRESIZE message");
		__android_log_print(ANDROID_LOG_INFO, "GHNative", "about to send WINDOWRESIZE message");
		GHMessage resizeMessage(GHMessageTypes::WINDOWRESIZE);
		mAppMessageQueue->handleMessage(resizeMessage);
	}
}

void GHAndroidAppRunner::onStart(void)
{
	GHDebugMessage::outputString("GHAndroidAppRunner::onStart() called");
	mEGLThread = new GHAndroidEGLThread(mSystemServices->getPlatformServices().getThreadFactory(), *this);
	mEGLThread->setWindow(mWindow);
}

void GHAndroidAppRunner::onStop(void)
{
	GHDebugMessage::outputString("GHAndroidAppRunner::onStop() called");
	delete mEGLThread;
	mEGLThread = 0;
}

void GHAndroidAppRunner::onPause(void)
{
	GHDebugMessage::outputString("Pause callback. mAppMessageQueue = %d", mAppMessageQueue);
	if (mAppMessageQueue) {
		mAppMessageQueue->handleMessage(GHMessage(GHMessageTypes::PAUSEINTERRUPT));
	}
	mEGLThread->pause();
}

void GHAndroidAppRunner::onResume(void)
{
	GHDebugMessage::outputString("Resume callback");
	if (mAppMessageQueue) {
		mAppMessageQueue->handleMessage(GHMessage(GHMessageTypes::UNPAUSEINTERRUPT));
	}
	mEGLThread->resume();
}

void GHAndroidAppRunner::setWindow(ANativeWindow* window, int width, int height)
{
	if (window && (width == 0 || height == 0))
	{
		// figure out window dimensions if they are not passed in.
		width = ANativeWindow_getWidth(window);
		height = ANativeWindow_getHeight(window);
	}

	bool handleResize = true;
	if (!window || width == 0 || height == 0)
	{
		// don't handle resize if we're going to 0.
		// could lead to nans on size(0,0).
		handleResize = false;
	}

	if (mWindow)
	{
		if (mWindow != window)
		{
			ANativeWindow_release(mWindow);
		}
	}

	mWindow = window;
	if (mEGLThread)
	{
		mEGLThread->setWindow(window);
	}

	if (window != 0)
	{
		mWindowWidth = width;
		mWindowHeight = height;
	}

	if (handleResize)
	{
		onResize(mWindowWidth, mWindowHeight);
	}
}

void GHAndroidAppRunner::handleTouchStart(int pointerId)
{
	//GHDebugMessage::outputString("touch start - %d", pointerId);
	if (!mVolatileInputState) return;

	mInputMutex->acquire();
	mVolatileInputState->handlePointerActive(pointerId, true);
	int mouseKey = (int)GHKeyDef::KEY_MOUSE1 + (pointerId*mVolatileInputState->getNumMouseButtons());
	mVolatileInputState->handleKeyChange(0, mouseKey, true);
	mInputMutex->release();
}

void GHAndroidAppRunner::handleTouchEnd(int pointerId)
{
	//GHDebugMessage::outputString("touch end - %d", pointerId);
	if (!mVolatileInputState) return;

	mInputMutex->acquire();
	int mouseKey = (int)GHKeyDef::KEY_MOUSE1 + (pointerId*mVolatileInputState->getNumMouseButtons());
	mVolatileInputState->handleKeyChange(0, mouseKey, false);
	mVolatileInputState->handlePointerChange(pointerId, GHPoint2(-1, -1));
	mVolatileInputState->handlePointerActive(pointerId, false);
	mInputMutex->release();
}

void GHAndroidAppRunner::handleTouchPos(int pointerId, float x, float y)
{
	if (!mVolatileInputState) return;

	mInputMutex->acquire();
	GHPoint2 normPos;
	normPos[0] = x / mRenderServices->getScreenInfo().getSize()[0];
	normPos[1] = y / mRenderServices->getScreenInfo().getSize()[1];
	//GHDebugMessage::outputString("touch move - %d (%f %f) --> (%f %f)", pointerId, x, y, normPos[0], normPos[1]);
	mVolatileInputState->handlePointerChange(pointerId, normPos);
	mInputMutex->release();
}

void GHAndroidAppRunner::handleAcceleration(float x, float y, float z)
{
	if (!mVolatileInputState) return;

	mInputMutex->acquire();
	//GHDebugMessage::outputString("Handling accel: %f %f %f", x, y, z);
	mVolatileInputState->handleAccelerometer(0, GHPoint3(x, y, z));
	mInputMutex->release();
}

void GHAndroidAppRunner::handleBackPressed(void)
{
	if (!mVolatileInputState) return;

	//GHDebugMessage::outputString("Handling back pressed.");
	mInputMutex->acquire();
	mVolatileInputState->handleKeyChange(0, (int)GHKeyDef::KEY_ESC, true);
	mVolatileInputState->handleKeyChange(0, (int)GHKeyDef::KEY_ESC, false);
	mInputMutex->release();
}

void GHAndroidAppRunner::handleKeyboardButtonChange(int key, float pressed)
{
	if (!mVolatileInputState) return;
	mInputMutex->acquire();
	mVolatileInputState->handleKeyChange(0, key, pressed);
	mInputMutex->release();
}

void GHAndroidAppRunner::handleGamepadButtonChange(int gamepadIdx, int key, float pressed)
{
	if (!mVolatileInputState) return;
	
	mInputMutex->acquire();
	mVolatileInputState->handleGamepadButtonChange(gamepadIdx, key, pressed);
	mInputMutex->release();
}

void GHAndroidAppRunner::handleGamepadJoystickChange(int gamepadIdx, int joyIdx, float x, float y)
{
	if (!mVolatileInputState) return;

	mInputMutex->acquire();
	mVolatileInputState->handleGamepadJoystickChange(gamepadIdx, joyIdx, GHPoint2(x, y));
	mInputMutex->release();
}

void GHAndroidAppRunner::handleGamepadActive(int gamepadIdx, bool active)
{
	if (!mVolatileInputState) return;

	mInputMutex->acquire();
	mVolatileInputState->handleGamepadActive(gamepadIdx, active);
	mInputMutex->release();
}
