// Copyright Golden Hammer Software
#include "GHDirectSoundEmitter.h"
#include "GHDirectSoundHandle.h"
#include "GHPlatform/GHDebugMessage.h"

GHDirectSoundEmitter::GHDirectSoundEmitter(GHDirectSoundHandle& handle, 
	const GHIdentifier& category, 
	GHSoundVolumeMgr& volumeMgr)
: GHSoundEmitter(category, volumeMgr)
, mHandle(handle)
, mLooping(false)
{
	mHandle.acquire();
}

GHDirectSoundEmitter::~GHDirectSoundEmitter(void)
{
	mHandle.release();
}

void GHDirectSoundEmitter::play(void)
{
	DWORD playFlags = 0;
	if (isLooping()) {
		playFlags = DSBPLAY_LOOPING;
	}
	HRESULT res = mHandle.getBuffer()->Play(0, 0, playFlags);
	if (!SUCCEEDED(res))
	{
		GHDebugMessage::outputString("Failed to play sound");
	}
}

void GHDirectSoundEmitter::pause(void)
{
	HRESULT stopRes = mHandle.getBuffer()->Stop();
	if (!SUCCEEDED(stopRes)) {
		GHDebugMessage::outputString("Failed to stop sound");
	}
}

void GHDirectSoundEmitter::stop(void)
{
	HRESULT stopRes = mHandle.getBuffer()->Stop();
	if (!SUCCEEDED(stopRes)) {
		GHDebugMessage::outputString("Failed to stop sound");
	}
	HRESULT posRes = mHandle.getBuffer()->SetCurrentPosition(0);
	if (!SUCCEEDED(posRes)) {
		GHDebugMessage::outputString("Failed to reset sound to 0");
	}
}

void GHDirectSoundEmitter::setPosition(const GHPoint3& pos)
{
	// todo
}

void GHDirectSoundEmitter::setVelocity(const GHPoint3& pos)
{
	// todo
}

void GHDirectSoundEmitter::setLooping(bool looping)
{
	mLooping = looping;
}

void GHDirectSoundEmitter::setPitch(float pitch)
{
	HRESULT res = mHandle.getBuffer()->SetFrequency((DWORD)(DSBFREQUENCY_ORIGINAL*pitch));
}

bool GHDirectSoundEmitter::isPlaying(void) const
{
	DWORD status;
	HRESULT res = mHandle.getBuffer()->GetStatus(&status);
	if (!SUCCEEDED(res)) {
		GHDebugMessage::outputString("Failed to get sound status");
		return false;
	}

	return (status & DSBSTATUS_PLAYING);
}

bool GHDirectSoundEmitter::isLooping(void) const
{
	return mLooping;
}

void GHDirectSoundEmitter::applyGain(float gain)
{
	float dbs = DSBVOLUME_MIN + float(DSBVOLUME_MAX - DSBVOLUME_MIN)*gain;
	HRESULT res = mHandle.getBuffer()->SetVolume((LONG)dbs);
	if (!SUCCEEDED(res)) {
		GHDebugMessage::outputString("Failed to apply gain");
	}
}

void GHDirectSoundEmitter::seek(float t)
{
	GHDebugMessage::outputString("Error, GHDirectSoundEmitter::setTemporalPosition not yet implemented");
}
