#include "GHAndroidMediaPlayer.h"
#include "GHPlatform/GHDebugMessage.h"
#include <ctime>
#include "GHPlatform/android/GHJNIMgr.h"

GHAndroidMediaPlayer::GHAndroidMediaPlayer(GHJNIMgr& jniMgr, jobject engineInterface)
: mJNIMgr(jniMgr)
, mEngineInterface(engineInterface)
{
}

void GHAndroidMediaPlayer::init(void)
{
    GHDebugMessage::outputString("In GHAndroidMediaPlayer::init");
    
	jclass engInterfaceCls = mJNIMgr.getJNIEnv().GetObjectClass(mEngineInterface);
	if(engInterfaceCls == 0) {
		GHDebugMessage::outputString("GHAndroidMediaPlayer can't find class where createMediaPlayer method should exist\n.");
	}
	mCreateMediaPlayer = mJNIMgr.getJNIEnv().GetMethodID(engInterfaceCls, "createMediaPlayer", "(Ljava/lang/String;)Lgoldenhammer/ghbase/GHMediaPlayerInfo;");
	GHDebugMessage::outputString("MediaPlayer Create Method: %d", mCreateMediaPlayer);

	mDestroyMediaPlayer = mJNIMgr.getJNIEnv().GetMethodID(engInterfaceCls, "destroyMediaPlayer", "(Lgoldenhammer/ghbase/GHMediaPlayerInfo;)V");
	GHDebugMessage::outputString("MediaPlayer Destroy Method: %d", mDestroyMediaPlayer);

	mClass = mJNIMgr.getJNIEnv().FindClass("goldenhammer/ghbase/GHMediaPlayerInfo");
	GHDebugMessage::outputString("MediaPlayer Class: %d", mClass);

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
    
    mIsPlaying = mJNIMgr.getJNIEnv().GetMethodID(mClass, "isPlaying", "()Z");
    GHDebugMessage::outputString("IsPlaying ID: %d", mIsPlaying);
    
    GHDebugMessage::outputString("Exiting GHAndroidMediaPlayer::init");
}

jobject GHAndroidMediaPlayer::createMediaPlayer(const char* filename)
{
	jstring jfilename = mJNIMgr.getJNIEnv().NewStringUTF(filename);
	jobject mediaPlayerObj = mJNIMgr.getJNIEnv().CallObjectMethod(mEngineInterface, mCreateMediaPlayer, jfilename);
	mJNIMgr.getJNIEnv().DeleteLocalRef(jfilename);

	jobject ret = mJNIMgr.getJNIEnv().NewGlobalRef(mediaPlayerObj);
	mJNIMgr.getJNIEnv().DeleteLocalRef(mediaPlayerObj);
	return ret;
}

void GHAndroidMediaPlayer::releaseMediaPlayer(jobject mediaPlayerObj)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mEngineInterface, mDestroyMediaPlayer, mediaPlayerObj);
	mJNIMgr.getJNIEnv().DeleteGlobalRef(mediaPlayerObj);
}

void GHAndroidMediaPlayer::start(jobject mediaPlayer)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mediaPlayer, mStart);
}

void GHAndroidMediaPlayer::pause(jobject mediaPlayer)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mediaPlayer, mPause);
}

void GHAndroidMediaPlayer::stop(jobject mediaPlayer)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mediaPlayer, mStop);
}

void GHAndroidMediaPlayer::setVolume(jobject mediaPlayer, float volume)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mediaPlayer, mSetVolume, volume, volume);
}

void GHAndroidMediaPlayer::setLooping(jobject mediaPlayer, bool looping)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mediaPlayer, mSetLooping, looping);
}

bool GHAndroidMediaPlayer::isPlaying(jobject mediaPlayer) const
{
    return mJNIMgr.getJNIEnv().CallBooleanMethod(mediaPlayer, mIsPlaying);
}
