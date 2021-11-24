// Copyright Golden Hammer Software
#pragma once

#include "Base/GHSoundDevice.h"
#include "GHXAudio2Include.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHUtils/GHController.h"
#include "GHPlatform/GHTimeCalculator.h"
#include <vector>
#include "GHMetro3DAudioMgr.h"
#include "GHMetroSoundErrorHandler.h"

class GHEventMgr;
class GHControllerMgr;
class GHMetroSoundEmitter;
class GHMetroSoundHandle;

class GHMetroSoundDevice : public GHSoundDevice
{
public:
	GHMetroSoundDevice(GHEventMgr& eventMgr, GHControllerMgr& controllerMgr, GHSoundVolumeMgr& volumeMgr, const GHTimeCalculator& timeCalc);
	~GHMetroSoundDevice(void);

	virtual void enable3DSound(bool enable);
	virtual void setListenerPosition(const GHPoint3& pos);
	virtual void setListenerOrientation(const GHTransform& orientation);
	virtual void setListenerVelocity(const GHPoint3& velocity);
	virtual GHSoundEmitter* createEmitter(GHSoundHandle& sound, const GHIdentifier& category);

	void shutdown(void);
	void init(void);

	void registerEmitter(GHMetroSoundEmitter* emitter);
	void unregisterEmitter(GHMetroSoundEmitter* emitter);
	IXAudio2SourceVoice* createSourceVoice(GHMetroSoundHandle& sound, IXAudio2VoiceCallback* callback);

	void pause(void);
	void unpause(void);

	UINT32 getNumInputChannels(void) const;

private:

	class PauseHandler : public GHMessageHandler
	{
	public:
		PauseHandler(GHMetroSoundDevice& soundDevice,
					GHEventMgr& eventMgr);
		virtual ~PauseHandler(void);

		virtual void handleMessage(const GHMessage& message);

	private:
		GHMetroSoundDevice& mSoundDevice;
		GHEventMgr& mEventMgr;
	};

private:
	class DeviceSelector
	{
	public:
		~DeviceSelector(void);
		const wchar_t* getCurrentlySelectedDevicePath(void) const;
		void setSelectedDevice(const wchar_t* guidStr);
	private:
		void clearDevicePath(void);
		wchar_t* mDevicePath{ 0 };
	};

	class DeviceStatusHandler : public GHMessageHandler
	{
	public:
		DeviceStatusHandler(GHMetroSoundErrorHandler& GHMetroSoundErrorHandler, DeviceSelector& deviceSelector, GHEventMgr& eventMgr);
		virtual ~DeviceStatusHandler(void);

		virtual void handleMessage(const GHMessage& message);

	private:
		GHMetroSoundErrorHandler& mErrorHandler;
		DeviceSelector& mDeviceSelector;
		GHEventMgr& mEventMgr;
	};

	class AudioCallback : public IXAudio2EngineCallback
	{
	public:
		AudioCallback(GHMetroSoundErrorHandler& GHMetroSoundErrorHandler);
		void _stdcall OnProcessingPassStart(){};
		void  _stdcall OnProcessingPassEnd(){};
		void  _stdcall OnCriticalError(HRESULT Error);

	private:
		GHMetroSoundErrorHandler& mErrorHandler;
	};

private:
	PauseHandler mPauseHandler;
	GHMetroSoundErrorHandler mErrorHandler;
	DeviceSelector mDeviceSelector;
	DeviceStatusHandler mDeviceStatusHandler;
	AudioCallback mCallback;
	IXAudio2* mAudioEngine;
	IXAudio2MasteringVoice* mMasteringVoice;
	int mNFrameToApply3DAudio;
	std::vector<GHMetroSoundEmitter*> mRegisteredEmitters;
	GHMetro3DAudioMgr m3DAudioMgr;
	bool mIsPaused;
	bool mNeedsReinitOnUnpause;
	GHSoundVolumeMgr& mVolumeMgr;
};
