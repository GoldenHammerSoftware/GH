// Copyright 2010 Golden Hammer Software
#include "GHAndroidSoundDevice.h"
#include "GHAndroidSoundPoolHandle.h"
#include "GHAndroidSoundEmitter.h"
#include "GHNullSoundEmitter.h"
#include "GHPlatform/android/GHJNIMgr.h"

GHAndroidSoundDevice::GHAndroidSoundDevice(GHJNIMgr& jniMgr, jobject engineInterface, GHEventMgr& eventMgr, 
	GHSoundVolumeMgr& volumeMgr)
: mJNIMgr(jniMgr)
, mMediaPlayer(jniMgr, engineInterface)
, mEventMgr(eventMgr)
, mNextEmitterGUID(0)
, mVolumeMgr(volumeMgr)
{
    mMediaPlayer.init();
}

GHAndroidSoundDevice::~GHAndroidSoundDevice(void)
{
    
}

void GHAndroidSoundDevice::setListenerPosition(const GHPoint3& pos)
{
    
}

GHSoundEmitter* GHAndroidSoundDevice::createEmitter(GHSoundHandle& sound, const GHIdentifier& category)
{
    GHAndroidSoundPoolHandle& androidHandle = reinterpret_cast<GHAndroidSoundPoolHandle&>(sound);
    return new GHAndroidSoundEmitter(androidHandle, mMediaPlayer, mEventMgr, mNextEmitterGUID++, category, mVolumeMgr);
}
