// Copyright 2010 Golden Hammer Software
#include "GHOALSoundEmitter.h"
#include "GHOALSoundHandleMgr.h"
#include "GHOALSoundHandle.h"
#include "GHPoint.h"
#include "GHOALErrorReporter.h"

GHOALSoundEmitter::GHOALSoundEmitter(const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr,
                                     GHOALSoundHandleMgr& handleMgr, GHSoundHandle& sound)
: GHSoundEmitter(category, volumeMgr)
, mHandleMgr(handleMgr)
, mSound(sound)
{
	mSound.acquire();
	
	ALfloat zeroPos[] = {0.0f, 0.0f, 0.0f};
	
	mEmitterId = mHandleMgr.getUnusedEmitterHandle();
	alSourcei(mEmitterId, AL_BUFFER, ((GHOALSoundHandle&)sound).getBufferId());
	alSourcef(mEmitterId, AL_PITCH, 1.0f);
	alSourcef(mEmitterId, AL_GAIN, 0.5f);
	alSourcefv(mEmitterId, AL_POSITION, zeroPos);
	alSourcefv(mEmitterId, AL_VELOCITY, zeroPos);
	alSourcei(mEmitterId, AL_LOOPING, false);
	GHOALErrorReporter::checkAndReportError("GHOALSoundEmitter::constructor");
    updateGain();
}

GHOALSoundEmitter::~GHOALSoundEmitter(void)
{
	alSourceStop(mEmitterId);
	alSourcei(mEmitterId, AL_BUFFER, 0);
	mSound.release();
	mHandleMgr.returnEmitterHandleToPool(mEmitterId);
	GHOALErrorReporter::checkAndReportError("OALGHOALSoundEmitter::destructor");
}

void GHOALSoundEmitter::play(void)
{
	alSourcePlay(mEmitterId);
	GHOALErrorReporter::checkAndReportError("OALGHOALSoundEmitter::play");
}

void GHOALSoundEmitter::pause(void)
{
	alSourcePause(mEmitterId);
	GHOALErrorReporter::checkAndReportError("OALGHOALSoundEmitter::pause");
}

void GHOALSoundEmitter::stop(void)
{
	alSourceStop(mEmitterId);
	GHOALErrorReporter::checkAndReportError("OALGHOALSoundEmitter::stop");
}

void GHOALSoundEmitter::setPosition(const GHPoint3& pos)
{
	alSourcefv(mEmitterId, AL_POSITION, pos.getCoords());
	GHOALErrorReporter::checkAndReportError("OALGHOALSoundEmitter::setPosition");
}

void GHOALSoundEmitter::setVelocity(const GHPoint3& pos)
{
	alSourcefv(mEmitterId, AL_VELOCITY, pos.getCoords());
	GHOALErrorReporter::checkAndReportError("OALGHOALSoundEmitter::setVelocity");
}

void GHOALSoundEmitter::setLooping(bool looping)
{
	alSourcei(mEmitterId, AL_LOOPING, looping);
	GHOALErrorReporter::checkAndReportError("OALGHOALSoundEmitter::setLooping");
}

void GHOALSoundEmitter::setPitch(float pitch)
{
	alSourcef(mEmitterId, AL_PITCH, pitch);
	GHOALErrorReporter::checkAndReportError("OALGHOALSoundEmitter::setPitch");
}

void GHOALSoundEmitter::seek(float t)
{
    //todo
}

void GHOALSoundEmitter::applyGain(float gain)
{
	alSourcef(mEmitterId, AL_GAIN, gain);
	GHOALErrorReporter::checkAndReportError("OALGHOALSoundEmitter::setGain");
}

bool GHOALSoundEmitter::isPlaying(void) const 
{
	ALenum state;
	alGetSourcei(mEmitterId, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

bool GHOALSoundEmitter::isLooping(void) const
{
    ALint looping = AL_FALSE;
    alGetSourcei(mEmitterId, AL_LOOPING, &looping);
    GHOALErrorReporter::checkAndReportError("OALGHOALSoundEmitter::getLooping");
    return (looping != AL_FALSE);
}
