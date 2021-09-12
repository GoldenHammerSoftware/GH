// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundEmitter.h"
#include "GHOALInclude.h"

class GHOALSoundHandleMgr;
class GHSoundHandle;

// An openAL class that plays a sound buffer at a location.
class GHOALSoundEmitter : public GHSoundEmitter
{
public:
	GHOALSoundEmitter(const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr,
                      GHOALSoundHandleMgr& handleMgr, GHSoundHandle& sound);
	~GHOALSoundEmitter(void);

	virtual void play(void);
	virtual void pause(void);
	virtual void stop(void);

	virtual void setPosition(const GHPoint3& pos);
	virtual void setVelocity(const GHPoint3& pos);
	virtual void setLooping(bool looping);
	virtual void setPitch(float pitch);
    virtual void seek(float t);
	virtual bool isPlaying(void) const;
    virtual bool isLooping(void) const;
    
    // todo
    virtual void setIsPositional(bool isPositional) {}
    virtual bool isPositional(void) const { return false; }

private:
    virtual void applyGain(float gain);
    
protected:
	GHOALSoundHandleMgr& mHandleMgr;
	GHSoundHandle& mSound;
	ALuint mEmitterId;
};
