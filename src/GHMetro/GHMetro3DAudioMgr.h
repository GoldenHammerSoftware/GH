#pragma once
#include "GHUtils/GHController.h"
#include "GHXAudio2Include.h"
#include <vector>
#include "GHMath/GHPoint.h"

class GHControllerMgr;
class GHMetroSoundEmitter;
class GHMetroSoundErrorHandler;
class GHTransform;

//Manages the 3D (positional) component of the sound system.
class GHMetro3DAudioMgr : public GHController
{
public:
	typedef std::vector<GHMetroSoundEmitter*> EmitterList;
public:
	GHMetro3DAudioMgr(GHControllerMgr& controllerMgr, 
					  EmitterList& registeredEmitters,
					  GHMetroSoundErrorHandler& errorHandler);
	~GHMetro3DAudioMgr(void);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	void init(IXAudio2* audioEngine, IXAudio2MasteringVoice* masteringVoice);

	void enable3DSound(bool enable);
	void setListenerPosition(const GHPoint3& pos);
	void setListenerOrientation(const GHTransform& orientation);
	void setListenerVelocity(const GHPoint3& velocity);
	UINT32 getNumInputChannels(void) const;

private:
	void update(GHMetroSoundEmitter& emitter);
	void retrieveChannelsAndSampleRate(IXAudio2* audioEngine, IXAudio2MasteringVoice* masteringVoice, DWORD& outChannelMask, UINT32& outChannels, UINT32& outSampleRate);
	void retrieveChannelsAndSampleRate(IXAudio2Voice* voice, UINT32& outChannels, UINT32& outSampleRate);

private:
	X3DAUDIO_HANDLE mX3DInstance;
	X3DAUDIO_LISTENER mX3DListener;
	X3DAUDIO_DSP_SETTINGS mDSPSettings{ 0 }; //not sure if this one should live here or on the emitters
	int mFrameThrottle{ 2 };
	int mFrameThrottleCounter{ 0 };

	GHControllerMgr& mControllerMgr;
	//belongs to GHMetroSoundDevice, but we need to iterate them
	EmitterList& mRegisteredEmitters;
	GHMetroSoundErrorHandler& mErrorHandler;
	IXAudio2MasteringVoice* mMasteringVoice{ 0 };
	bool mIsEnabled{ false };
};
