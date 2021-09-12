#pragma once

#include <jni.h>
class GHJNIMgr;

class GHAndroidSoundPool
{
public:
	GHAndroidSoundPool(GHJNIMgr& jniMgr, jobject engineInterface);
	virtual ~GHAndroidSoundPool() { }
	virtual void init(void);
	
	virtual jobject createSound(const char* filename);
	virtual void releaseSound(jobject mediaPlayer);

	virtual void start(jobject mediaPlayer);
	virtual void pause(jobject mediaPlayer);
	virtual void stop(jobject mediaPlayer);

	virtual void setVolume(jobject mediaPlayer, float volume);
	virtual void setLooping(jobject mediaPlayer, bool looping);
    virtual void setPitch(jobject mediaPlayer, float pitch);
    
    bool isPlaying(jobject mediaPlayer) const;
    
    void registerEmitter(unsigned int guid, jobject mediaPlayer);
    void unregisterEmitter(unsigned int guid, jobject mediaPlayer);

protected:
	GHJNIMgr& mJNIMgr;
	jobject mEngineInterface;
	jmethodID mCreateSound;
	jmethodID mDestroySound;

	jclass mClass;
	jmethodID mStart;
	jmethodID mStop;
	jmethodID mPause;
	jmethodID mSetVolume;
	jmethodID mSetLooping;
    jmethodID mSetPitch;
    jmethodID mIsPlaying;
};
