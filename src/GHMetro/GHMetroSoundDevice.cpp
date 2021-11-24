// Copyright Golden Hammer Software
#include "GHMetroSoundDevice.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMetroSoundHandle.h"
#include "GHMetroSoundEmitter.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHUtils/GHEventMgr.h"
#include "Base/GHMessageTypes.h"
#include "GHUtils/GHMessage.h"
#include "Base/GHBaseIdentifiers.h"
#include "GHWin32DeviceFinder.h"

GHMetroSoundDevice::GHMetroSoundDevice(GHEventMgr& eventMgr, GHControllerMgr& controllerManager, 
	GHSoundVolumeMgr& volumeMgr, const GHTimeCalculator& time)
: mPauseHandler(*this, eventMgr)
, mErrorHandler(*this, controllerManager, time, eventMgr)
, mDeviceStatusHandler(mErrorHandler, mDeviceSelector, eventMgr)
, m3DAudioMgr(controllerManager, mRegisteredEmitters, mErrorHandler)
, mCallback(mErrorHandler)
, mAudioEngine(0)
, mMasteringVoice(0)
, mIsPaused(false)
, mNeedsReinitOnUnpause(false)
, mVolumeMgr(volumeMgr)
{
	HRESULT result = MFStartup(MF_VERSION);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("Failed to start Windows sound engine.");
		return;
	}

	try
	{
		init();
	}
	catch(...)
	{
	}
}

void GHMetroSoundDevice::init(void)
{
	if (mIsPaused)
	{
		mNeedsReinitOnUnpause = true;
		return;
	}

	UINT32 flags = 0;
	HRESULT result = XAudio2Create(&mAudioEngine, flags);
	mErrorHandler.checkResult(result, "XAudio2Create");
	if (!mAudioEngine) {
		return;
	}
	result = mAudioEngine->RegisterForCallbacks(&mCallback);
	mErrorHandler.checkResult(result, "XAudio2 RegisterForCallbacks");

	//*
    XAUDIO2_DEBUG_CONFIGURATION debugConfig = {0};
	debugConfig.BreakMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_DETAIL | XAUDIO2_LOG_WARNINGS;
	debugConfig.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_DETAIL | XAUDIO2_LOG_WARNINGS;
    mAudioEngine->SetDebugConfiguration(&debugConfig);
	//*/

//#define FORCE_DEFAULT_SOUND_DEVICE 1
#ifndef FORCE_DEFAULT_SOUND_DEVICE
	result = mAudioEngine->CreateMasteringVoice( &mMasteringVoice, 
												 XAUDIO2_DEFAULT_CHANNELS, 
												 XAUDIO2_DEFAULT_SAMPLERATE,
												 0,
												 mDeviceSelector.getCurrentlySelectedDevicePath( ) );
	if (result == E_FAIL)
	{
		const wchar_t* devicePath = mDeviceSelector.getCurrentlySelectedDevicePath();
		if (!devicePath)
		{
			devicePath = L"";
		}
		GHDebugMessage::outputString("IXAudio2::CreateMasteringVoice failed trying to intialize on device with path %ls. Trying again on default device", devicePath);

		result = mAudioEngine->CreateMasteringVoice( &mMasteringVoice, 
													 XAUDIO2_DEFAULT_CHANNELS, 
													 XAUDIO2_DEFAULT_SAMPLERATE );
	}
#else
	result = mAudioEngine->CreateMasteringVoice(&mMasteringVoice,
		XAUDIO2_DEFAULT_CHANNELS,
		XAUDIO2_DEFAULT_SAMPLERATE);
#endif

	mErrorHandler.checkResult(result, "IXAudio2::CreateMasteringVoice");

	m3DAudioMgr.init(mAudioEngine, mMasteringVoice);

	for (size_t i = 0; i < mRegisteredEmitters.size(); ++i)
	{
		GHMetroSoundEmitter* emitter = mRegisteredEmitters[i];
		emitter->init();
	}
}

UINT32 GHMetroSoundDevice::getNumInputChannels(void) const
{
	//this isn't necessarily 3D-specific, but we are storing it there
	return m3DAudioMgr.getNumInputChannels();
}

void GHMetroSoundDevice::shutdown(void)
{
	for (size_t i = 0; i < mRegisteredEmitters.size(); ++i)
	{
		GHMetroSoundEmitter* emitter = mRegisteredEmitters[i];
		emitter->shutdown();
	}

	if (mMasteringVoice)
	{
		mMasteringVoice->DestroyVoice();
	}
	if (mAudioEngine)
	{
		mAudioEngine->Release();
	}
	mMasteringVoice = 0;
	mAudioEngine = 0;
}

GHMetroSoundDevice::~GHMetroSoundDevice(void)
{
	try
	{
		shutdown();
	}
	catch(...)
	{
	}
}

void GHMetroSoundDevice::enable3DSound(bool enable)
{
	m3DAudioMgr.enable3DSound(false);
}

void GHMetroSoundDevice::setListenerPosition(const GHPoint3& pos)
{
	m3DAudioMgr.setListenerPosition(pos);
}

void GHMetroSoundDevice::setListenerOrientation(const GHTransform& orientation)
{
	m3DAudioMgr.setListenerOrientation(orientation);
}

void GHMetroSoundDevice::setListenerVelocity(const GHPoint3& velocity)
{
	m3DAudioMgr.setListenerVelocity(velocity);
}

IXAudio2SourceVoice* GHMetroSoundDevice::createSourceVoice(GHMetroSoundHandle& sound, IXAudio2VoiceCallback* callback)
{
	if (!mAudioEngine) {
		return 0;
	}

	const int numSendDescriptors = 1;
	XAUDIO2_SEND_DESCRIPTOR sendDescriptors[numSendDescriptors];
	sendDescriptors[0].Flags = XAUDIO2_SEND_USEFILTER; // LPF direct-path
	sendDescriptors[0].pOutputVoice = mMasteringVoice;

	const XAUDIO2_VOICE_SENDS sendList = { numSendDescriptors, sendDescriptors };

	IXAudio2SourceVoice* ret = 0;
	mAudioEngine->CreateSourceVoice(&ret, &sound.getFormat(), 0, 2.0, callback, &sendList);
	return ret;
}

GHSoundEmitter* GHMetroSoundDevice::createEmitter(GHSoundHandle& sound, const GHIdentifier& category)
{
	GHMetroSoundHandle& soundHandle = (GHMetroSoundHandle&)sound;	
	return new GHMetroSoundEmitter(*this, soundHandle, category, mVolumeMgr);
}

void GHMetroSoundDevice::registerEmitter(GHMetroSoundEmitter* emitter)
{
	mRegisteredEmitters.push_back(emitter);
}

void GHMetroSoundDevice::unregisterEmitter(GHMetroSoundEmitter* emitter)
{
	std::vector<GHMetroSoundEmitter*>::iterator iter = std::find(mRegisteredEmitters.begin(), mRegisteredEmitters.end(), emitter);
	if (iter != mRegisteredEmitters.end())
	{
		mRegisteredEmitters.erase(iter);
	}
}

GHMetroSoundDevice::PauseHandler::PauseHandler(GHMetroSoundDevice& soundDevice,
					GHEventMgr& eventMgr)
					: mSoundDevice(soundDevice)
					, mEventMgr(eventMgr)
{
	mEventMgr.registerListener(GHMessageTypes::PAUSEINTERRUPT, *this);
	mEventMgr.registerListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
}

GHMetroSoundDevice::PauseHandler::~PauseHandler(void)
{
	mEventMgr.unregisterListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
	mEventMgr.unregisterListener(GHMessageTypes::PAUSEINTERRUPT, *this);
}

void GHMetroSoundDevice::pause(void)
{
	if (mAudioEngine) {
		mAudioEngine->StopEngine();
	}
	mIsPaused = true;
}

void GHMetroSoundDevice::unpause(void)
{
	mIsPaused = false;
	if (mNeedsReinitOnUnpause)
	{
		try
		{
			init();
		}
		catch(...)
		{
		}
	}
	else
	{
		if (mAudioEngine) {
			mAudioEngine->StartEngine();
		}
	}
}

GHMetroSoundDevice::AudioCallback::AudioCallback(GHMetroSoundErrorHandler& errorHandler)
	: mErrorHandler(errorHandler)
{
}

void GHMetroSoundDevice::AudioCallback::OnCriticalError(HRESULT error)
{
	mErrorHandler.onCriticalError();
}

void GHMetroSoundDevice::PauseHandler::handleMessage(const GHMessage& message)
{
	if (message.getType() == GHMessageTypes::PAUSEINTERRUPT)
	{
		mSoundDevice.pause();
	}
	else if (message.getType() == GHMessageTypes::UNPAUSEINTERRUPT)
	{
		mSoundDevice.unpause();
	}
}

GHMetroSoundDevice::DeviceSelector::~DeviceSelector(void)
{
	clearDevicePath();
}

void GHMetroSoundDevice::DeviceSelector::clearDevicePath(void)
{
	delete[] mDevicePath;
	mDevicePath = 0;
}

const wchar_t* GHMetroSoundDevice::DeviceSelector::getCurrentlySelectedDevicePath(void) const
{
	return mDevicePath;
}

void GHMetroSoundDevice::DeviceSelector::setSelectedDevice(const wchar_t* guidStr)
{
	const static size_t kBufferSize = 512;
	clearDevicePath();
	mDevicePath = new wchar_t[kBufferSize];

	GHWin32DeviceFinder deviceFinder;
	if (!deviceFinder.findAudioDevicePath(guidStr, mDevicePath, kBufferSize))
	{
		clearDevicePath();
	}
}

GHMetroSoundDevice::DeviceStatusHandler::DeviceStatusHandler(GHMetroSoundErrorHandler& errorHandler, DeviceSelector& deviceSelector, GHEventMgr& eventMgr)
	: mErrorHandler(errorHandler)
	, mDeviceSelector(deviceSelector)
	, mEventMgr(eventMgr)
{
	mEventMgr.registerListener(GHBaseIdentifiers::M_NEWAUDIODEVICEAVAILABLE, *this);
}

GHMetroSoundDevice::DeviceStatusHandler::~DeviceStatusHandler(void)
{
	mEventMgr.unregisterListener(GHBaseIdentifiers::M_NEWAUDIODEVICEAVAILABLE, *this);
}

void GHMetroSoundDevice::DeviceStatusHandler::handleMessage(const GHMessage& message)
{
	const GHProperty& deviceGuidProp = message.getPayload().getProperty(GHBaseIdentifiers::MP_DESIREDAUDIODEVICEGUIDSTR);
	if (deviceGuidProp.isValid())
	{
		const wchar_t* deviceGuid = deviceGuidProp;
		mDeviceSelector.setSelectedDevice(deviceGuid);
	}

	//we reinit the audio system in onCritcalError.
	// We also want to do this when the new device (ie headphones) is available
	mErrorHandler.onCriticalError();
}
