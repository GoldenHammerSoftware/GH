// Shared code between GH android main.cpp files

#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "GHString/GHString.h"

#include "GHAndroidAppRunner.h"

// *** FOR A NEW APP ***
// Include this file in your jni main.cpp, and implement this function.
GHAndroidAppRunner* createAndroidAppRunner(JNIEnv* env, JavaVM* jvm, jobject javaEngineInterface, jobject jAssetMgr, std::vector<GHString>&& saveFileDirectories, jobject activity);

// global pointers to keep things in scope for our lifecycle.
static JavaVM* sJVM = 0;
static jobject sGlobalThiz = 0;
static jobject sGlobalEngineInterface = 0;
static jobject sGlobalAssetMgr = 0;
static jobject sGlobalActivity = 0;

static GHAndroidAppRunner* sAndroidAppRunner = 0;

static bool sUseIAP = false;

jint JNI_OnLoad(JavaVM* aVm, void* aReserved)
{
	// cache java VM
	sJVM = aVm;
	return JNI_VERSION_1_6;
}

// *** BEGIN JAVA LIFECYCLE

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleJavaCreate(JNIEnv* env, jobject thiz, jobject engineInterface, jobject jAssetMgr, jobjectArray jSaveFileDirectories, jobject activity)
{
	// grab global pointers to keep stuff around.
	sGlobalThiz = env->NewGlobalRef(thiz);
	sGlobalEngineInterface = env->NewGlobalRef(engineInterface);
	sGlobalActivity = env->NewGlobalRef(activity);

	if (jAssetMgr) {
		sGlobalAssetMgr = env->NewGlobalRef(jAssetMgr);
	}

	std::vector<GHString> saveFileDirectories;
	int numDirectories = env->GetArrayLength(jSaveFileDirectories);
	saveFileDirectories.reserve(numDirectories);
	for (int i = 0; i < numDirectories; ++i)
	{
		jstring jSaveFileDirectory = (jstring)(env->GetObjectArrayElement(jSaveFileDirectories, i));
		//Note: we used to call strdup on the result of getStringUTFChars. 
		// I think this was a leak, since we already duplicate the result to store as a GHString (this copy moved here from several calls down in GHAndroidFileOpener).
		// Additionally, we were calling ReleaseStringUTFChars on the result of strdup, which I think is technically the wrong method (should be free)
		// (and we do need to call ReleaseStringUTFChars on the original result of GetStringUTFChars).
		const char* cdirectory = env->GetStringUTFChars(jSaveFileDirectory, 0);
		saveFileDirectories.push_back(GHString(cdirectory, GHString::CHT_COPY));
		env->ReleaseStringUTFChars(jSaveFileDirectory, cdirectory);
	}

	sAndroidAppRunner = createAndroidAppRunner(env, sJVM, sGlobalEngineInterface, sGlobalAssetMgr, std::move(saveFileDirectories), sGlobalActivity);
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleJavaStart(JNIEnv* env, jobject thiz)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->setUseIAP(sUseIAP);
	sAndroidAppRunner->onStart();
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleJavaPause(JNIEnv* env, jobject thiz)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->onPause();
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleJavaResume(JNIEnv* env, jobject thiz)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->onResume();
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleJavaStop(JNIEnv* env, jobject thiz)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->onStop();
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleJavaShutdown(JNIEnv* env, jobject thiz)
{
	delete sAndroidAppRunner; sAndroidAppRunner = 0;
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleSetSurface(JNIEnv* env, jobject thiz, jobject surface, jint width, jint height)
{
	if (!sAndroidAppRunner) {
		GHDebugMessage::outputString("handleSetSurface called before handleJavaCreate!");
		return;
	}
	if (!surface)
	{
		sAndroidAppRunner->setWindow(0, width, height);
		return;
	}
	//ANativeWindow_fromSurface calls ANativeWindow_acquire() so we need to call ANativeWindow_release().
	ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
	sAndroidAppRunner->setWindow(window, width, height);
}

// *** END JAVA LIFECYCLE

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleTouchStart(JNIEnv* env, jobject thiz, jint touchId)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->handleTouchStart(touchId);
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleTouchEnd(JNIEnv* env, jobject thiz, jint touchId)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->handleTouchEnd(touchId);
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleTouchPos(JNIEnv* env, jobject thiz, jint touchId, float x, float y)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->handleTouchPos(touchId, x, y);
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleBackPressed(JNIEnv* env, jobject thiz)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->handleBackPressed();
}

extern "C"
__attribute__((visibility("default"))) 
void
Java_goldenhammer_ghbase_GHEngineInterface_handleAcceleration(JNIEnv* env, jobject thiz, float x, float y, float z)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->handleAcceleration(x,y,z);
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleKeyboardButtonChange(JNIEnv* env, jobject thiz, int key, float pressed)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->handleKeyboardButtonChange(key, pressed);
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleGamepadButtonChange(JNIEnv* env, jobject thiz, int gamepadIdx, int key, float pressed)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->handleGamepadButtonChange(gamepadIdx, key, pressed);
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleGamepadJoystickChange(JNIEnv* env, jobject thiz, int gamepadIdx, int joyIdx, float x, float y)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->handleGamepadJoystickChange(gamepadIdx, joyIdx, x, y);
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleGamepadActive(JNIEnv* env, jobject thiz, int gamepadIdx, int active)
{
	if (!sAndroidAppRunner) return;
	sAndroidAppRunner->handleGamepadActive(gamepadIdx, active > 0);
}

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_setSupportsIAP(JNIEnv* env, jobject thiz, jint supportsIAP)
{
	sUseIAP = (supportsIAP > 0) ? true : false;
}
