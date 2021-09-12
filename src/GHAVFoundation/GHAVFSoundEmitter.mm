// Copyright 2010 Golden Hammer Software
#include "GHAVFSoundEmitter.h"
#include <AVFoundation/AVFoundation.h>
#include "GHAVFSoundHandle.h"
#include "GHAVFSoundEmitterMgr.h"

GHAVFSoundEmitter::GHAVFSoundEmitter(GHAVFSoundHandle& sound, GHAVFSoundEmitterMgr& emitterMgr,
                                     const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr)
: GHSoundEmitter(category, volumeMgr)
, mEmitterMgr(emitterMgr)
, mSound(sound)
{
	mSound.acquire();
	updateGain();
}

GHAVFSoundEmitter::~GHAVFSoundEmitter(void)
{
	mSound.release();
}

void GHAVFSoundEmitter::play(void)
{
	if (mEmitterMgr.registerPlayingSound(this)) 
	{
		[mSound.getAudioPlayer() prepareToPlay];
		[mSound.getAudioPlayer() play];
	}
}

void GHAVFSoundEmitter::pause(void)
{
	[mSound.getAudioPlayer() pause];
}

void GHAVFSoundEmitter::stop(void)
{
	[mSound.getAudioPlayer() stop];
    mSound.getAudioPlayer().currentTime = 0;
	mEmitterMgr.unregisterPlayingSound(this);
}

void GHAVFSoundEmitter::setPosition(const GHPoint3& pos)
{
}

void GHAVFSoundEmitter::setVelocity(const GHPoint3& pos)
{
}

void GHAVFSoundEmitter::setLooping(bool looping)
{
	if(looping) {
		[mSound.getAudioPlayer() setNumberOfLoops:-1];
	}
	else {
		[mSound.getAudioPlayer() setNumberOfLoops:0];
	}
}

void GHAVFSoundEmitter::setPitch(float pitch)
{
}

void GHAVFSoundEmitter::seek(float t)
{
    //todo
}

void GHAVFSoundEmitter::applyGain(float gain)
{
	[mSound.getAudioPlayer() setVolume:gain];
}

bool GHAVFSoundEmitter::isPlaying(void) const
{
	return [mSound.getAudioPlayer() isPlaying] == YES;
}

bool GHAVFSoundEmitter::isLooping(void) const
{
    return [mSound.getAudioPlayer() numberOfLoops] != 0;
}

