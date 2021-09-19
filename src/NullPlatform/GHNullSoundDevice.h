// Copyright Golden Hammer Software
#pragma once

#include "Base/GHSoundDevice.h"
class GHSoundVolumeMgr;

class GHNullSoundDevice : public GHSoundDevice
{
public:
    GHNullSoundDevice(GHSoundVolumeMgr& volumeMgr);
    
	virtual void enable3DSound(bool enable);
    virtual void setListenerPosition(const GHPoint3& pos);
	virtual void setListenerOrientation(const GHTransform& orientation);
	virtual void setListenerVelocity(const GHPoint3& velocity);
	virtual GHSoundEmitter* createEmitter(GHSoundHandle& sound, const GHIdentifier& category);
    
private:
    GHSoundVolumeMgr& mVolumeMgr;
};
