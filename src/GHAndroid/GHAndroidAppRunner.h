#pragma once

#include <string.h>
#include <jni.h>
#include "GHUtils/GHControllerMgr.h"
#include <vector>
#include "GHString/GHString.h"

class GHSystemServices;
class GHInputState;
class GHMessageQueue;
class GHAndroidRenderServices;
class GHGameServices;
class GHIAPStoreFactory;
class GHStatTrackerFactory;
class GHMultiplayerFactory;
class GHAdFactory;
class GHMutex;
class GHAppRunner;
class GHApp;
class GHJNIMgr;
class ANativeWindow;
class GHAndroidEGLThread;
class GHMessage;

// wrapper for calls from jni to run the android app.
class GHAndroidAppRunner
{
public:
	// onCreate
	GHAndroidAppRunner(JNIEnv* env, JavaVM* jvm, jobject javaEngineInterface, jobject jAssetMgr, jobject activity, std::vector<GHString>&& saveFileDirectories);
	// onDestroy
	virtual ~GHAndroidAppRunner(void);

	// implement this to specify a GHApp instance.
	virtual void createApp(void) = 0;

	void onTick(void);
	void sendAppMessages(void);
	void handleImmediateAppMessage(const GHMessage& message);

	void onStart(void);
	void onStop(void);
	void onPause(void);
	void onResume(void);
	void setWindow(ANativeWindow* window, int width, int height);

	void handleTouchStart(int pointerId);
	void handleTouchEnd(int pointerId);
	void handleTouchPos(int pointerId, float x, float y);
	void handleAcceleration(float x, float y, float z);
	void handleBackPressed();

	void handleKeyboardButtonChange(int key, float pressed);
	void handleGamepadButtonChange(int gamepadIdx, int key, float pressed);
	void handleGamepadJoystickChange(int gamepadIdx, int joyIdx, float x, float y);
	void handleGamepadActive(int gamepadIdx, bool active);

	void setUseIAP(bool val) { mUseIAP = val; }

private:
	void launchNativeApp(int windowWidth, int windowHeight);
	void onResize(int width, int height);

protected:
	bool mUseIAP{ false };

	jobject mJavaEngineInterface;

	// currently exists because we don't have
	// a controller mgr before the app shard exists.
	GHControllerMgr mSystemControllerMgr;

	GHSystemServices* mSystemServices{ 0 };
	GHInputState* mVolatileInputState{ 0 };
	GHMessageQueue* mAppMessageQueue{ 0 };
	GHAndroidRenderServices* mRenderServices{ 0 };
	GHGameServices* mGameServices{ 0 };
	GHIAPStoreFactory* mIAPStoreFactory{ 0 };
	GHStatTrackerFactory* mStatTrackerFactory{ 0 };
	GHMultiplayerFactory* mMultiplayerFactory{ 0 };
	GHAdFactory* mAdFactory{ 0 };
	GHMutex* mInputMutex{ 0 };
	GHAppRunner* mAppRunner{ 0 };
	GHApp* mApp{ 0 };
	GHAndroidEGLThread* mEGLThread{ 0 };

	ANativeWindow* mWindow{ 0 };
	int mWindowWidth{ 0 };
	int mWindowHeight{ 0 };

	GHJNIMgr* mJNIMgr{ 0 };
};
