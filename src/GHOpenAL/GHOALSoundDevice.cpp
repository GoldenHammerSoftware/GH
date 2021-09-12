// Copyright 2010 Golden Hammer Software
#include "GHOALSoundDevice.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOALErrorReporter.h"
#include "GHPoint.h"
#include "GHOALSoundEmitter.h"
#include "GHOALSoundHandleMgr.h"
#include "GHOALSoundHandle.h"
#include "GHUtils/GHEventMgr.h"
#include "GHOALInterruptHandler.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHTransition.h"

GHOALSoundDevice::GHOALSoundDevice(GHEventMgr& eventMgr, GHTransition* platformSoundPauseTransition,
                                   GHSoundVolumeMgr& volumeMgr)
: mContext(0)
, mDevice(0)
, mSoundHandleMgr(0)
, mEventManager(eventMgr)
, mPlatformSoundPauseTransition(platformSoundPauseTransition)
, mInterruptHandler(new GHOALInterruptHandler(*this))
, mVolumeMgr(volumeMgr)
{
	mDevice = alcOpenDevice(0);
	GHOALErrorReporter::checkAndReportError("alcOpenDevice");
	if (mDevice == 0) {
		GHDebugMessage::outputString("Failed to initialize OpenAL device.\n");
		return; 
	}
	
	// Create an OpenAL Context
	createContext();
	if (!mContext) {
		GHDebugMessage::outputString("Failed to initialize OpenAL context.\n");
		return;
	}
	
	alcMakeContextCurrent(mContext);
	GHOALErrorReporter::checkAndReportError("alcMakeContextCurrent");
    
    mSoundHandleMgr = new GHOALSoundHandleMgr(16, 200);
	
	GHPoint3 zero3(0,0,0);
	setListenerPosition(zero3);
	
	GHDebugMessage::outputString("OpenAL sound system initialized.");
	
    mEventManager.registerListener(GHMessageTypes::PAUSEINTERRUPT, *mInterruptHandler);
	mEventManager.registerListener(GHMessageTypes::UNPAUSEINTERRUPT, *mInterruptHandler);
}

GHOALSoundDevice::~GHOALSoundDevice(void)
{
    mEventManager.unregisterListener(GHMessageTypes::UNPAUSEINTERRUPT, *mInterruptHandler); 
    mEventManager.unregisterListener(GHMessageTypes::PAUSEINTERRUPT, *mInterruptHandler);
    delete mSoundHandleMgr;
    delete mInterruptHandler;
	destroyContext();
	if (mDevice) alcCloseDevice(mDevice);	
    delete mPlatformSoundPauseTransition;
}

void GHOALSoundDevice::setListenerPosition(const GHPoint3& pos)
{
	alListener3f(AL_POSITION, pos[0], pos[1], pos[2]);
	GHOALErrorReporter::checkAndReportError("alListener3f");
	
	// todo:
// 	alListenerfv(AL_VELOCITY,listenerVel);
//	alListenerfv(AL_ORIENTATION,listenerOri);	
}

GHSoundEmitter* GHOALSoundDevice::createEmitter(GHSoundHandle& sound, const GHIdentifier& category)
{
	return new GHOALSoundEmitter(category, mVolumeMgr, *mSoundHandleMgr, sound);
}

void GHOALSoundDevice::createContext(void)
{
	mContext = alcCreateContext(mDevice, 0);
	GHOALErrorReporter::checkAndReportError("alcCreateContext");
}

void GHOALSoundDevice::destroyContext(void)
{
	if(mContext) alcDestroyContext(mContext);
	mContext = 0;
}

void GHOALSoundDevice::handlePause(void)
{
	float gain = 0.0f;
	alListenerfv(AL_GAIN, &gain);

//	alcMakeContextCurrent(0);
//	ErrorReporter::checkAndReportError("MakeContextCurrent 0");
	alcSuspendContext(mContext);
	GHOALErrorReporter::checkAndReportError("SuspendContext");
	if (mPlatformSoundPauseTransition) { mPlatformSoundPauseTransition->deactivate(); }
	
	//destroyContext();

	GHOALErrorReporter::checkAndReportError("handleBeginPauseInterrupt - end");
}

void GHOALSoundDevice::handleUnpause(void)
{
	GHOALErrorReporter::checkAndReportError("handleEndPauseInterrupt - start");

	//createContext();
	//RestoreMyContextState(mContext);

	if (mPlatformSoundPauseTransition) { mPlatformSoundPauseTransition->activate(); }
	alcProcessContext(mContext);
	GHOALErrorReporter::checkAndReportError("ProcessContext");
//	alcMakeContextCurrent(mContext);
//	ErrorReporter::checkAndReportError("MakeContextCurrent mContext");

	float gain = 1.0f;
	alListenerfv(AL_GAIN, &gain);
}
