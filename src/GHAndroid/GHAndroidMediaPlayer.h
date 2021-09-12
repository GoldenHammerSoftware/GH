#pragma once

#include <jni.h>
class GHJNIMgr;

class GHAndroidMediaPlayer
{
public:
	GHAndroidMediaPlayer(GHJNIMgr& jniMgr, jobject engineInterface);
	virtual ~GHAndroidMediaPlayer() { }
	virtual void init(void);
	
	virtual jobject createMediaPlayer(const char* filename);
	virtual void releaseMediaPlayer(jobject mediaPlayer);

	virtual void start(jobject mediaPlayer);
	virtual void pause(jobject mediaPlayer);
	virtual void stop(jobject mediaPlayer);

	virtual void setVolume(jobject mediaPlayer, float volume);
	virtual void setLooping(jobject mediaPlayer, bool looping);
    
    bool isPlaying(jobject mediaPlayer) const;

protected:
	GHJNIMgr& mJNIMgr;
	jobject mEngineInterface;
	jmethodID mCreateMediaPlayer;
	jmethodID mDestroyMediaPlayer;

	jclass mClass;
	jmethodID mStart;
	jmethodID mStop;
	jmethodID mPause;
	jmethodID mSetVolume;
	jmethodID mSetLooping;
    jmethodID mIsPlaying;
};