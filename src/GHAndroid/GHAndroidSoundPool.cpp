#include "GHAndroidSoundPool.h"
#include "GHPlatform/GHDebugMessage.h"
#include <ctime>
#include "GHUtils/GHProfiler.h"
#include "GHPlatform/android/GHJNIMgr.h"

GHAndroidSoundPool::GHAndroidSoundPool(GHJNIMgr& jniMgr, jobject engineInterface)
: mJNIMgr(jniMgr)
, mEngineInterface(engineInterface)
{
}

void GHAndroidSoundPool::init(void)
{
    GHDebugMessage::outputString("In GHAndroidSoundPool::init");
    
	jclass engInterfaceCls = mJNIMgr.getJNIEnv().GetObjectClass(mEngineInterface);
	if(engInterfaceCls == 0) {
		GHDebugMessage::outputString("GHAndroidSoundPool can't find class where createMediaPlayer method should exist\n.");
	}
    
    jmethodID initMethod = mJNIMgr.getJNIEnv().GetMethodID(engInterfaceCls, "initializeSoundPool", "()V");
    GHDebugMessage::outputString("SoundPool init method: %d", initMethod);
	mJNIMgr.getJNIEnv().CallVoidMethod(mEngineInterface, initMethod);
    
	mCreateSound = mJNIMgr.getJNIEnv().GetMethodID(engInterfaceCls, "createSoundPoolSound", "(Ljava/lang/String;)Lgoldenhammer/ghbase/GHSoundPoolSound;");
	GHDebugMessage::outputString("SoundPool Sound Create Method: %d", mCreateSound);

	mDestroySound = mJNIMgr.getJNIEnv().GetMethodID(engInterfaceCls, "destroySoundPoolSound", "(Lgoldenhammer/ghbase/GHSoundPoolSound;)V");
	GHDebugMessage::outputString("SoundPool Sound Destroy Method: %d", mDestroySound);

	mClass = mJNIMgr.getJNIEnv().FindClass("goldenhammer/ghbase/GHSoundPoolSound");
	GHDebugMessage::outputString("SoundPool Sound Class: %d", mClass);

	mStart = mJNIMgr.getJNIEnv().GetMethodID(mClass, "start", "()V");
	GHDebugMessage::outputString("Start ID: %d", mStart);

	mStop = mJNIMgr.getJNIEnv().GetMethodID(mClass, "stop", "()V");
	GHDebugMessage::outputString("Stop ID: %d", mStop);

	mPause = mJNIMgr.getJNIEnv().GetMethodID(mClass, "pause", "()V");
	GHDebugMessage::outputString("Pause ID: %d", mPause);

	mSetVolume = mJNIMgr.getJNIEnv().GetMethodID(mClass, "setVolume", "(FF)V");
	GHDebugMessage::outputString("SetVolume ID: %d", mSetVolume);

	mSetLooping = mJNIMgr.getJNIEnv().GetMethodID(mClass, "setLooping", "(Z)V");
	GHDebugMessage::outputString("SetLooping ID: %d", mSetLooping);
    
    mSetPitch = mJNIMgr.getJNIEnv().GetMethodID(mClass, "setPitch", "(F)V");
    GHDebugMessage::outputString("SetPitch ID: %d", mSetPitch);
    
    mIsPlaying = mJNIMgr.getJNIEnv().GetMethodID(mClass, "isPlaying", "()Z");
    GHDebugMessage::outputString("IsPlaying ID: %d", mIsPlaying);
    
    GHDebugMessage::outputString("Exiting GHAndroidSoundPool::init");
}

void GHAndroidSoundPool::registerEmitter(unsigned int guid, jobject mediaPlayer)
{
    
}

void GHAndroidSoundPool::unregisterEmitter(unsigned int guid, jobject mediaPlayer)
{
    
}

jobject GHAndroidSoundPool::createSound(const char* filename)
{
	//GHDebugMessage::outputString("GHAndroidSoundPool::createSound called");
    GHPROFILESCOPE("GHAndroidSoundPool::createSound", GHString::CHT_REFERENCE);

	jstring jfilename = mJNIMgr.getJNIEnv().NewStringUTF(filename);
	jobject mediaPlayerObj = mJNIMgr.getJNIEnv().CallObjectMethod(mEngineInterface, mCreateSound, jfilename);
	mJNIMgr.getJNIEnv().DeleteLocalRef(jfilename);
    jobject ret = mJNIMgr.getJNIEnv().NewGlobalRef(mediaPlayerObj);
	mJNIMgr.getJNIEnv().DeleteLocalRef(mediaPlayerObj);
	//GHDebugMessage::outputString("GHAndroidSoundPool::createSound finished");
    return ret;
}

void GHAndroidSoundPool::releaseSound(jobject mediaPlayerObj)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mEngineInterface, mDestroySound, mediaPlayerObj);
	mJNIMgr.getJNIEnv().DeleteGlobalRef(mediaPlayerObj);
}

void GHAndroidSoundPool::start(jobject mediaPlayer)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mediaPlayer, mStart);
}

void GHAndroidSoundPool::pause(jobject mediaPlayer)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mediaPlayer, mPause);
}

void GHAndroidSoundPool::stop(jobject mediaPlayer)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mediaPlayer, mStop);
}

void GHAndroidSoundPool::setVolume(jobject mediaPlayer, float volume)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mediaPlayer, mSetVolume, volume, volume);
}

void GHAndroidSoundPool::setLooping(jobject mediaPlayer, bool looping)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mediaPlayer, mSetLooping, looping);
}

void GHAndroidSoundPool::setPitch(jobject mediaPlayer, float pitch)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mediaPlayer, mSetPitch, pitch);
}

bool GHAndroidSoundPool::isPlaying(jobject mediaPlayer) const
{
    return mJNIMgr.getJNIEnv().CallBooleanMethod(mediaPlayer, mIsPlaying);
}
