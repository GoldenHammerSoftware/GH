// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundDevice.h"
#include "GHAVFSoundEmitterMgr.h"

class GHEventMgr;

// ios AVFoundation sound device.
class GHAVFSoundDevice : public GHSoundDevice
{
public:
	GHAVFSoundDevice(GHEventMgr& eventMgr, GHSoundVolumeMgr& volumeMgr);
	~GHAVFSoundDevice(void);
	
	virtual void setListenerPosition(const GHPoint3& pos);
	virtual GHSoundEmitter* createEmitter(GHSoundHandle& sound, const GHIdentifier& category);
    // todo
    virtual void enable3DSound(bool enable) {}
    virtual void setListenerOrientation(const GHTransform& orientation) {}
    virtual void setListenerVelocity(const GHPoint3& velocity) {}

private:
	GHAVFSoundEmitterMgr mEmitterMgr;
    GHSoundVolumeMgr& mVolumeMgr;
};
