// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHString/GHIdentifier.h"

class GHSoundVolumeMgr;

// interface for something that can play a sound at a location.
// intended to be created through the GHSoundDevice::createEmitter(sound handle) func
class GHSoundEmitter 
{
public:
    GHSoundEmitter(const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr);
    virtual ~GHSoundEmitter(void);

	virtual void play(void) = 0;
	virtual void pause(void) = 0;
	virtual void stop(void) = 0;

	virtual void setIsPositional(bool isPositional) = 0;
	virtual void setPosition(const GHPoint3& pos) = 0;
	virtual void setVelocity(const GHPoint3& pos) = 0;
	virtual void setLooping(bool looping) = 0;
	virtual void setPitch(float pitch) = 0;
	virtual void seek(float t) = 0;

    // take a raw gain and modify it by category then applyGain.
	void setGain(float gain);
    // react to a change in global volume.
    void updateGain(void);
    
	virtual bool isPositional(void) const = 0;
	virtual bool isPlaying(void) const = 0;
    virtual bool isLooping(void) const = 0;
    
private:
    // take a gain that has already been modified by the category volume and apply it.
    virtual void applyGain(float gain) = 0;
    
private:
    // the category of our volume.
    // 0 means global only.
    GHIdentifier mCategory;
    // The raw value of gain before we apply category volume.
    float mUnmodifiedGain;
    GHSoundVolumeMgr& mVolumeMgr;
};
