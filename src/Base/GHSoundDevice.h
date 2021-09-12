// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHSoundVolumeMgr.h"

class GHSoundEmitter;
class GHSoundHandle;
class GHIdentifier;
class GHTransform;

// an interface class for something that can play sounds.
class GHSoundDevice 
{
public:
    virtual ~GHSoundDevice(void) {}
    
	virtual void enable3DSound(bool enable) = 0;
	virtual void setListenerPosition(const GHPoint3& pos) = 0;
	virtual void setListenerOrientation(const GHTransform& orientation) = 0;
	virtual void setListenerVelocity(const GHPoint3& velocity) = 0;
    // category is used to modify gain.
	virtual GHSoundEmitter* createEmitter(GHSoundHandle& sound, const GHIdentifier& category) = 0;
};
