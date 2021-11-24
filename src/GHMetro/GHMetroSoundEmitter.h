// Copyright Golden Hammer Software
#pragma once

#include "Base/GHSoundEmitter.h"
#include "GHXAudio2Include.h"

class GHMetroSoundHandle;
class GHMetroSoundDevice;

class GHMetroSoundEmitter : public GHSoundEmitter
{
public:
	GHMetroSoundEmitter(GHMetroSoundDevice& soundDevice,
						GHMetroSoundHandle& soundHandle,
						const GHIdentifier& category,
						GHSoundVolumeMgr& volumeMgr);
	~GHMetroSoundEmitter(void);

	virtual void play(void);
	virtual void pause(void);
	virtual void stop(void);

	virtual void setIsPositional(bool isPositional);
	virtual void setPosition(const GHPoint3& pos);
	virtual void setVelocity(const GHPoint3& pos);
	virtual void setLooping(bool looping);
	virtual void setPitch(float pitch);
	virtual bool isPositional(void) const;
	virtual bool isPlaying(void) const;
    virtual bool isLooping(void) const;
	virtual void seek(float t);

	void init(void);
	void shutdown(void);

	X3DAUDIO_EMITTER& getX3DAudioEmitter(void) { return m3DEmitter; }
	IXAudio2SourceVoice* getSourceVoice(void) { return mSourceVoice; }

private:
	virtual void applyGain(float gain);

private:
	class SoundCallback : public IXAudio2VoiceCallback
	{
	public:
		SoundCallback(bool& isPlayingFlag) : mIsPlayingFlag(isPlayingFlag) { }
		virtual void STDMETHODCALLTYPE OnBufferStart(void *pBufferContext) { }
		virtual void STDMETHODCALLTYPE OnBufferEnd(void *pBufferContext) { }
		virtual void STDMETHODCALLTYPE OnLoopEnd(void *pBufferContext) { }
		virtual void STDMETHODCALLTYPE OnStreamEnd(void) { mIsPlayingFlag = false; }
		virtual void STDMETHODCALLTYPE OnVoiceError(void *pBufferContext, HRESULT error);
		virtual void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 BytesRequired) { }
		virtual void STDMETHODCALLTYPE OnVoiceProcessingPassEnd(void) { }
	private:
		bool& mIsPlayingFlag;
	};

private:
	GHMetroSoundDevice& mSoundDevice;
	GHMetroSoundHandle& mSoundHandle;
	IXAudio2SourceVoice* mSourceVoice;
	XAUDIO2_VOICE_STATE mVoiceState;
	X3DAUDIO_EMITTER m3DEmitter;
	X3DAUDIO_CONE m3DCone;
	DirectX::XMFLOAT3 mEmitterPos;
	XAUDIO2_BUFFER mPlayBuffer;
	bool mIsPositional;
	bool mIsPlaying;
	bool mIsPaused;
	SoundCallback mCallback; //must appear after mIsPlaying
	float mCurrentGain;
};