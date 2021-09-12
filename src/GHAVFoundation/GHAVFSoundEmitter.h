// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundEmitter.h"

class GHAVFSoundHandle;
class GHAVFSoundEmitterMgr;

// An AVAudioPlayer class that plays a sound buffer at a location.
class GHAVFSoundEmitter : public GHSoundEmitter
{
public:
	GHAVFSoundEmitter(GHAVFSoundHandle& sound, GHAVFSoundEmitterMgr& soundEmitterMgr,
                      const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr);
	~GHAVFSoundEmitter(void);
		
	virtual void play(void);
	virtual void pause(void);
	virtual void stop(void);
	
	virtual void setPosition(const GHPoint3& pos);
	virtual void setVelocity(const GHPoint3& pos);
	virtual void setLooping(bool looping);
	virtual void setPitch(float pitch);
	virtual bool isPlaying(void) const;
    virtual bool isLooping(void) const;
    virtual void seek(float t);
	
    // todo:
    virtual void setIsPositional(bool isPositional) {}
    virtual bool isPositional(void) const { return false; }
    
private:
    virtual void applyGain(float gain);
    
protected:
	GHAVFSoundEmitterMgr& mEmitterMgr;
	GHAVFSoundHandle& mSound;
};
