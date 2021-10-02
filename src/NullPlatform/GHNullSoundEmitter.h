// Copyright Golden Hammer Software
#pragma once

#include "Base/GHSoundEmitter.h"

class GHNullSoundEmitter : public GHSoundEmitter
{
public:
    GHNullSoundEmitter(const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr)
    : GHSoundEmitter(category, volumeMgr)
    {}
    
  	virtual void play(void) { }
	virtual void pause(void) { }
	virtual void stop(void) { }
    
	virtual void setIsPositional(bool) {}
	virtual void setPosition(const GHPoint3&) { }
	virtual void setVelocity(const GHPoint3&) { }
	virtual void setLooping(bool) { }
	virtual void setPitch(float) { }
	virtual void seek(float) { }
	virtual bool isPositional(void) const { return false; }
	virtual bool isPlaying(void) const { return false; }  
    virtual bool isLooping(void) const { return false; }
    
private:
    virtual void applyGain(float) {}
};
