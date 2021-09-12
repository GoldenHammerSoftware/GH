// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundDevice.h"
#include "GHiOSSoundTransition.h"
#include "GHOALSoundDevice.h"

class GHAVFSoundDevice;
class GHEventMgr;

class GHiOSSoundDevice : public GHSoundDevice
{
public:
	GHiOSSoundDevice(bool itunesPlaying, GHEventMgr& eventMgr, GHSoundVolumeMgr& volumeMgr);
	~GHiOSSoundDevice(void);
	
	virtual void setListenerPosition(const GHPoint3& pos);
	virtual GHSoundEmitter* createEmitter(GHSoundHandle& sound, const GHIdentifier& category);
    // todo
    virtual void enable3DSound(bool enable) {}
    virtual void setListenerOrientation(const GHTransform& orientation) {}
    virtual void setListenerVelocity(const GHPoint3& velocity) {}

	GHOALSoundDevice& getOALDevice(void) { return mOALDevice; }
	
    bool canPlayMusic(void) const { return mAVFDevice != 0; }
    
protected:
	GHiOSSoundTransition mPlatformSoundTransition;
	GHOALSoundDevice mOALDevice;
	GHAVFSoundDevice* mAVFDevice;
};
