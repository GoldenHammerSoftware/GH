// Copyright 2010 Golden Hammer Software
#include "GHOpenSLEmitter.h"
#include "GHOpenSLHandle.h"
#include "GHPlatform/GHDebugMessage.h"

GHOpenSLEmitter::GHOpenSLEmitter(const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr,
	GHOpenSLHandle& soundHandle, SLObjectItf& outputMixObject, SLEngineItf& engineEngine)
: GHSoundEmitter(category, volumeMgr)
, mSoundHandle(soundHandle)
, mOutputMixObject(outputMixObject)
, mEngineEngine(engineEngine)
, mPlayerObject(0)
, mPlayerPlay(0)
, mPlayerSeek(0)
, mPlayerMuteSolo(0)
, mPlayerVolume(0)
, mIsLooping(false)
{
	SLresult result;

	// configure audio sink
	mLocOutmix = { SL_DATALOCATOR_OUTPUTMIX, mOutputMixObject };
	mAudioSnk = { &mLocOutmix, NULL };

	// create audio player
	GHDebugMessage::outputString("GHOpenSLEmitter CreateAudioPlayer");
	const SLInterfaceID ids[3] = { SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME };
	const SLboolean req[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
	result = (*mEngineEngine)->CreateAudioPlayer(mEngineEngine, &mPlayerObject, &mSoundHandle.getDataSource(), &mAudioSnk,
		3, ids, req);
	if (result != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to create openSL audio player");

		if (result == SL_RESULT_PRECONDITIONS_VIOLATED) GHDebugMessage::outputString("SL_RESULT_PRECONDITIONS_VIOLATED");
		else if (result == SL_RESULT_PARAMETER_INVALID) GHDebugMessage::outputString("SL_RESULT_PARAMETER_INVALID");
		else if (result == SL_RESULT_MEMORY_FAILURE) GHDebugMessage::outputString("SL_RESULT_MEMORY_FAILURE");
		else if (SL_RESULT_IO_ERROR) GHDebugMessage::outputString("SL_RESULT_IO_ERROR");
		else if (SL_RESULT_CONTENT_CORRUPTED) GHDebugMessage::outputString("SL_RESULT_CONTENT_CORRUPTED");
		else if (SL_RESULT_CONTENT_UNSUPPORTED) GHDebugMessage::outputString("SL_RESULT_CONTENT_UNSUPPORTED");
		else if (SL_RESULT_CONTENT_NOT_FOUND) GHDebugMessage::outputString("SL_RESULT_CONTENT_NOT_FOUND");
		else if (SL_RESULT_PERMISSION_DENIED) GHDebugMessage::outputString("SL_RESULT_PERMISSION_DENIED");

		return;
	}

	// realize the player
	GHDebugMessage::outputString("GHOpenSLEmitter Realize mPlayerObject");
	result = (*mPlayerObject)->Realize(mPlayerObject, SL_BOOLEAN_FALSE);
	if (result != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to realize openSL audio player");
		return;
	}

	// get the play interface
	result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_PLAY, &mPlayerPlay);
	if (result != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to create openSL player object");
	}

	// get the seek interface
	result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_SEEK, &mPlayerSeek);
	if (result != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to create openSL player seek");
	}

	// get the mute/solo interface
	result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_MUTESOLO, &mPlayerMuteSolo);
	if (result != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to create openSL player mute solo");
	}

	// get the volume interface
	result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_VOLUME, &mPlayerVolume);
	if (result != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to create openSL player volume");
	}
	GHDebugMessage::outputString("GHOpenSLEmitter::GHOpenSLEmitter finished");
}

GHOpenSLEmitter::~GHOpenSLEmitter(void)
{
	GHDebugMessage::outputString("GHOpenSLEmitter Destroy(mPlayerObject)");

	if (mPlayerObject) (*mPlayerObject)->Destroy(mPlayerObject);
	mPlayerObject = 0;
	mPlayerPlay = 0;
	mPlayerSeek = 0;
	mPlayerMuteSolo = 0;
	mPlayerVolume = 0;

	GHDebugMessage::outputString("GHOpenSLEmitter destructor finished.");
}

void GHOpenSLEmitter::play(void)
{
	// make sure the asset audio player was created
	if (mPlayerPlay == 0) {
		GHDebugMessage::outputString("Trying to call play on null openSL object");
		return;
	}

	// set the player's state
	SLresult result = (*mPlayerPlay)->SetPlayState(mPlayerPlay, SL_PLAYSTATE_PLAYING);
	if (result != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to set openSL object playing");
	}
}

void GHOpenSLEmitter::pause(void)
{
	// make sure the asset audio player was created
	if (mPlayerPlay == 0) {
		GHDebugMessage::outputString("Trying to call pause on null openSL object");
		return;
	}

	// set the player's state
	SLresult result = (*mPlayerPlay)->SetPlayState(mPlayerPlay, SL_PLAYSTATE_PAUSED);
	if (result != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to set openSL object paused");
	}
}

void GHOpenSLEmitter::stop(void)
{
	// make sure the asset audio player was created
	if (mPlayerPlay == 0) {
		GHDebugMessage::outputString("Trying to call stop on null openSL object");
		return;
	}

	// set the player's state
	SLresult result = (*mPlayerPlay)->SetPlayState(mPlayerPlay, SL_PLAYSTATE_STOPPED);
	if (result != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to set openSL object stopped");
	}
}

void GHOpenSLEmitter::setPosition(const GHPoint3& pos)
{
	GHDebugMessage::outputString("Unimplemented openSL setPosition");
}

void GHOpenSLEmitter::setVelocity(const GHPoint3& pos)
{
	GHDebugMessage::outputString("Unimplemented openSL setVelocity");
}

void GHOpenSLEmitter::setLooping(bool looping)
{
	if (!mPlayerSeek)
	{
		GHDebugMessage::outputString("Called GHOpenSLEmitter::setLooping with no mPlayerSeek");
		return;
	}

	// enable whole file looping
	SLresult result;
	if (looping)
	{
		result = (*mPlayerSeek)->SetLoop(mPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
	}
	else
	{
		result = (*mPlayerSeek)->SetLoop(mPlayerSeek, SL_BOOLEAN_FALSE, 0, SL_TIME_UNKNOWN);
	}
	if (result != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to openSL setLooping");
	}
	mIsLooping = looping;
}

bool GHOpenSLEmitter::isLooping(void) const
{
	return mIsLooping;
}

void GHOpenSLEmitter::setPitch(float pitch)
{
	GHDebugMessage::outputString("Unimplemented openSL setPitch");
}

void GHOpenSLEmitter::seek(float t)
{
	if (mPlayerSeek == 0)
	{
		GHDebugMessage::outputString("openSL seek called with null mPlayerSeek");
		return;
	}
	GHDebugMessage::outputString("openSL seek not implemented.");
}

bool GHOpenSLEmitter::isPlaying(void) const
{
	if (mPlayerPlay == 0) {
		GHDebugMessage::outputString("Called GHOpenSLEmitter::isPlaying with no mPlayerPlay");
		return false;
	}

	SLresult result;
	SLuint32 state;
	result = (*mPlayerPlay)->GetPlayState(mPlayerPlay, &state);
	if (result != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to get opensl play state");
		return false;
	}

	return (state == SL_PLAYSTATE_PLAYING);
}

void GHOpenSLEmitter::applyGain(float gain)
{
	if (mPlayerVolume == 0)
	{
		GHDebugMessage::outputString("openSL applyGain called with no mPlayerVolume");
		return;
	}

	// https://groups.google.com/forum/#!topic/android-ndk/wOJCxFg5rf8
	//get min & max
	SLmillibel MinVolume = SL_MILLIBEL_MIN;
	SLmillibel MaxVolume = SL_MILLIBEL_MIN;
	(*mPlayerVolume)->GetMaxVolumeLevel(mPlayerVolume, &MaxVolume);

	//calc SLES volume
	SLmillibel Volume = MinVolume + (SLmillibel)(((float)(MaxVolume - MinVolume))*gain);

	//set
	(*mPlayerVolume)->SetVolumeLevel(mPlayerVolume, Volume);
}
