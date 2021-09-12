// Copyright 2010 Golden Hammer Software
#include "GHOpenSLDevice.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOpenSLEmitter.h"

GHOpenSLDevice::GHOpenSLDevice(GHSoundVolumeMgr& volumeMgr)
: mVolumeMgr(volumeMgr)
, mEngineObject(0)
, mOutputMixObject(0)
, mOutputMixEnvironmentalReverb(0)
{
	// initialize opensl
	SLresult res;
	res = slCreateEngine(&mEngineObject, 0, NULL, 0, NULL, NULL);
	if (res != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to create SL engine object");
	}

	res = (*mEngineObject)->Realize(mEngineObject, SL_BOOLEAN_FALSE);
	if (res != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to realize SL engine object");
	}

	res = (*mEngineObject)->GetInterface(mEngineObject, SL_IID_ENGINE, &mEngineEngine);
	if (res != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to get SL engine engine interface");
	}

	// create output mix, with environmental reverb specified as a non-required interface
	const SLInterfaceID ids[1] = { SL_IID_ENVIRONMENTALREVERB };
	const SLboolean req[1] = { SL_BOOLEAN_FALSE };
	res = (*mEngineEngine)->CreateOutputMix(mEngineEngine, &mOutputMixObject, 1, ids, req);
	if (res != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to create SL output mix");
	}

	// realize the output mix
	res = (*mOutputMixObject)->Realize(mOutputMixObject, SL_BOOLEAN_FALSE);
	if (res != SL_RESULT_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to realize SL output mix");
	}
}

GHOpenSLDevice::~GHOpenSLDevice(void)
{
	// destroy output mix object, and invalidate all associated interfaces
	GHDebugMessage::outputString("GHOpenSLDevice Destroy(mOutputMixObject");
	if (mOutputMixObject != NULL) {
		(*mOutputMixObject)->Destroy(mOutputMixObject);
		mOutputMixObject = NULL;
		mOutputMixEnvironmentalReverb = NULL;
	}
	// destroy engine object, and invalidate all associated interfaces
	GHDebugMessage::outputString("GHOpenSLDevice Destroy(mEngineObject)");
	if (mEngineObject != NULL) {
		(*mEngineObject)->Destroy(mEngineObject);
		mEngineObject = NULL;
		mEngineEngine = NULL;
	}
	GHDebugMessage::outputString("GHOpenSLDevice destructor finished");
}

void GHOpenSLDevice::setListenerPosition(const GHPoint3& pos)
{
}

GHSoundEmitter* GHOpenSLDevice::createEmitter(GHSoundHandle& sound, const GHIdentifier& category)
{
	return new GHOpenSLEmitter(category, mVolumeMgr, (GHOpenSLHandle&)sound, mOutputMixObject, mEngineEngine);
}
