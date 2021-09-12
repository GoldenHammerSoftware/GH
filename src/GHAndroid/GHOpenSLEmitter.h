// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundEmitter.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class GHOpenSLHandle;

// interface for something that can play a sound at a location.
// intended to be created through the GHSoundDevice::createEmitter(sound handle) func
class GHOpenSLEmitter : public GHSoundEmitter
{
public:
	GHOpenSLEmitter(const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr, 
		GHOpenSLHandle& soundHandle, SLObjectItf& outputMixObject, SLEngineItf& engineEngine);
	virtual ~GHOpenSLEmitter(void);

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

private:
	GHOpenSLHandle& mSoundHandle;
	// global output mix
	SLObjectItf& mOutputMixObject;
	// global engine engine.  sample code calls it this.
	SLEngineItf& mEngineEngine;

	SLDataLocator_OutputMix mLocOutmix;
	SLDataSink mAudioSnk;
	SLObjectItf mPlayerObject;
	SLPlayItf mPlayerPlay;
	SLSeekItf mPlayerSeek;
	SLMuteSoloItf mPlayerMuteSolo;
	SLVolumeItf mPlayerVolume;
	bool mIsLooping;
};
