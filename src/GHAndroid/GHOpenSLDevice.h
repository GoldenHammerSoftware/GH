// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundDevice.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// an interface class for something that can play sounds.
// ndk sample code https://android.googlesource.com/platform/development/+/c21a505/ndk/platforms/android-9/samples/native-audio/jni/native-audio-jni.c
class GHOpenSLDevice : public GHSoundDevice
{
public:
	GHOpenSLDevice(GHSoundVolumeMgr& volumeMgr);
	virtual ~GHOpenSLDevice(void);

	virtual void setListenerPosition(const GHPoint3& pos);
	virtual GHSoundEmitter* createEmitter(GHSoundHandle& sound, const GHIdentifier& category);

	SLObjectItf& getOutputMix(void) { return mOutputMixObject; }
	SLEngineItf& getEngineEngine(void) { return mEngineEngine; }

	// todo
	virtual void enable3DSound(bool val) {}
	virtual void setListenerOrientation(const GHTransform& orientation) {}
	virtual void setListenerVelocity(const GHPoint3& velocity) {}

private:
	GHSoundVolumeMgr& mVolumeMgr;

	SLObjectItf mEngineObject;
	SLEngineItf mEngineEngine;

	SLObjectItf mOutputMixObject;
	SLEnvironmentalReverbItf mOutputMixEnvironmentalReverb;
};
