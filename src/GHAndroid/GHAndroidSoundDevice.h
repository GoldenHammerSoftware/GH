// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundDevice.h"
#include <jni.h>
#include "GHAndroidSoundPool.h"

class GHJNIMgr;
class GHEventMgr;

class GHAndroidSoundDevice : public GHSoundDevice
{
public:
	GHAndroidSoundDevice(GHJNIMgr& jniMgr, jobject engineInterface, GHEventMgr& eventMgr,
                         GHSoundVolumeMgr& volumeMgr);
	~GHAndroidSoundDevice(void);
	
	virtual void setListenerPosition(const GHPoint3& pos);
	virtual GHSoundEmitter* createEmitter(GHSoundHandle& sound, const GHIdentifier& category);
    
    GHAndroidSoundPool& getMediaPlayer(void) { return mMediaPlayer; }

	// todo
	virtual void enable3DSound(bool val) {}
	virtual void setListenerOrientation(const GHTransform& orientation) {}
	virtual void setListenerVelocity(const GHPoint3& velocity) {}

private:
	GHJNIMgr& mJNIMgr;
    GHEventMgr& mEventMgr;
    GHAndroidSoundPool mMediaPlayer;
    int mNextEmitterGUID;
    GHSoundVolumeMgr& mVolumeMgr;
};
