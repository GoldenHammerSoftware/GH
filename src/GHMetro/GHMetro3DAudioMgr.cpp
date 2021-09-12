#include "GHMetro3DAudioMgr.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHMetroSoundErrorHandler.h"
#include "GHMetroSoundEmitter.h"
#include "GHMath/GHTransform.h"

//todo: make these dynamic
//constexpr unsigned int INPUTCHANNELS = 1;
//constexpr unsigned int OUTPUTCHANNELS = 2;

constexpr unsigned int BUFFERZONE = 64;

static const X3DAUDIO_CONE Listener_DirectionalCone = { X3DAUDIO_PI*5.0f / 6.0f, X3DAUDIO_PI*11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };

GHMetro3DAudioMgr::GHMetro3DAudioMgr(GHControllerMgr& controllerMgr, 
									 EmitterList& registeredEmitters,
									 GHMetroSoundErrorHandler& errorHandler)
	: mControllerMgr(controllerMgr)
	, mRegisteredEmitters(registeredEmitters)
	, mErrorHandler(errorHandler)
{
	ZeroMemory(&mDSPSettings, sizeof(mDSPSettings));
	ZeroMemory(&mX3DListener, sizeof(mX3DListener));

	setListenerOrientation(GHTransform::IDENTITY);

	mX3DListener.pCone = (X3DAUDIO_CONE*)&Listener_DirectionalCone;

	mControllerMgr.add(this);
}

GHMetro3DAudioMgr::~GHMetro3DAudioMgr(void)
{
	mControllerMgr.remove(this);

	delete[] mDSPSettings.pMatrixCoefficients;
}

void GHMetro3DAudioMgr::init(IXAudio2* audioEngine, IXAudio2MasteringVoice* masteringVoice)
{
	mMasteringVoice = masteringVoice;

	DWORD channelMask;
	UINT32 channels, sampleRate;
	retrieveChannelsAndSampleRate(audioEngine, masteringVoice, channelMask, channels, sampleRate);


	DWORD dwChannelMask;
	HRESULT result = masteringVoice->GetChannelMask(&dwChannelMask);
	mErrorHandler.checkResult(result, "IXAudio2MasteringVoice::GetChannelMask");

	result = X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, mX3DInstance);
	mErrorHandler.checkResult(result, "X3DAudioInitialize");

	//channels = 1;

	//todo: make this dynamic. Make sure to also update mDSPSettings.pMatrixCoefficients
	mDSPSettings.SrcChannelCount = 1;
	mDSPSettings.DstChannelCount = channels;

	delete[] mDSPSettings.pMatrixCoefficients;
	size_t matrixSize = mDSPSettings.SrcChannelCount*mDSPSettings.DstChannelCount + BUFFERZONE;
	mDSPSettings.pMatrixCoefficients = new FLOAT32[matrixSize];
	ZeroMemory(mDSPSettings.pMatrixCoefficients, sizeof(FLOAT32)*(matrixSize));

	delete[] mDSPSettings.pDelayTimes;
	size_t delayTimeSize = mDSPSettings.DstChannelCount + BUFFERZONE;
	mDSPSettings.pDelayTimes = new FLOAT32[delayTimeSize];
	ZeroMemory(mDSPSettings.pDelayTimes, sizeof(FLOAT32)*(delayTimeSize));
}

void GHMetro3DAudioMgr::onActivate(void)
{

}

void GHMetro3DAudioMgr::onDeactivate(void)
{

}

void GHMetro3DAudioMgr::update(void)
{
	if (!mIsEnabled) { return; }

	//we don't need to update every frame
	if (++mFrameThrottleCounter % mFrameThrottle != 0) {
		return;
	}

	//todo: convert between gh and directx units

	size_t numEmitters = mRegisteredEmitters.size();
	for (size_t i = 0; i < numEmitters; ++i)
	{
		auto emitter = mRegisteredEmitters[i];
		update(*emitter);
	}
}

void GHMetro3DAudioMgr::update(GHMetroSoundEmitter& emitter)
{
	if (!emitter.isPositional())
	{
		return;
	}

	X3DAUDIO_EMITTER* x3DEmitter = &emitter.getX3DAudioEmitter();
	IXAudio2SourceVoice* sourceVoice = emitter.getSourceVoice();

	UINT32 channels, sampleRate;
	retrieveChannelsAndSampleRate(sourceVoice, channels, sampleRate);

	assert(channels == x3DEmitter->ChannelCount);

	mDSPSettings.SrcChannelCount = x3DEmitter->ChannelCount;

	//if (x3DEmitter->ChannelCount == 2) return;

	UINT32 flags = X3DAUDIO_CALCULATE_MATRIX;
	//DWORD flags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
	//				| X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
	//				| X3DAUDIO_CALCULATE_REVERB;
	X3DAudioCalculate(mX3DInstance, &mX3DListener, x3DEmitter, flags, &mDSPSettings);
	
	HRESULT result = sourceVoice->SetOutputMatrix(mMasteringVoice, mDSPSettings.SrcChannelCount, mDSPSettings.DstChannelCount, mDSPSettings.pMatrixCoefficients);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("Error 0x%x in IXAudio2SourceVoice::SetOutputMatrix", result);
	}


	//todo: need to create the source voice with the correct XAUDIO3_SEND_DESCRIPTOR list
	XAUDIO2_FILTER_PARAMETERS FilterParametersDirect = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * mDSPSettings.LPFDirectCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
	//result = sourceVoice->SetOutputFilterParameters(mMasteringVoice, &FilterParametersDirect);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("Error 0x%x in IXAudio2SourceVoice::SetOutputFilterParamaters", result);
	}

	//todo?
	//voice->SetOutputMatrix(g_audioState.pSubmixVoice, 1, 1, &g_audioState.dspSettings.ReverbLevel);
	//XAUDIO2_FILTER_PARAMETERS FilterParametersReverb = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * g_audioState.dspSettings.LPFReverbCoefficient), 1.0f }; // see XAudio2CutoffFrequencyToRadians() in XAudio2.h for more information on the formula used here
	//voice->SetOutputFilterParameters(g_audioState.pSubmixVoice, &FilterParametersReverb);
}

void setXAudioVector(X3DAUDIO_VECTOR& xAudioVec, const GHPoint3& ghVec)
{
	xAudioVec.x = ghVec[0];
	xAudioVec.y = ghVec[1];
	xAudioVec.z = ghVec[2];
}

void GHMetro3DAudioMgr::enable3DSound(bool enable)
{
	mIsEnabled = enable;
}

void GHMetro3DAudioMgr::setListenerPosition(const GHPoint3& pos)
{
	setXAudioVector(mX3DListener.Position, pos);
}

void GHMetro3DAudioMgr::setListenerOrientation(const GHTransform& orientation)
{
	GHPoint3 orientFront(0, 0, -1);
	GHPoint3 orientTop(0, 1, 0);

	orientation.multDir(orientFront, orientFront);
	orientation.multDir(orientTop, orientTop);

	setXAudioVector(mX3DListener.OrientFront, orientFront);
	setXAudioVector(mX3DListener.OrientTop, orientTop);
}

void GHMetro3DAudioMgr::setListenerVelocity(const GHPoint3& velocity)
{
	setXAudioVector(mX3DListener.Velocity, velocity);
}

UINT32 GHMetro3DAudioMgr::getNumInputChannels(void) const
{
	return mDSPSettings.SrcChannelCount;
}

void GHMetro3DAudioMgr::retrieveChannelsAndSampleRate(IXAudio2* audioEngine, IXAudio2MasteringVoice* masteringVoice, DWORD& outChannelMask, UINT32& outChannels, UINT32& outSampleRate)
{
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)

	retrieveChannelsAndSampleRate(masteringVoice, outChannels, outSampleRate);

	HRESULT hr = masteringVoice->GetChannelMask(&outChannelMask);
	mErrorHandler.checkResult(hr, "IXAudio2MasteringVoice::GetChannelMask");

#else

	XAUDIO2_DEVICE_DETAILS details;
	HRESULT hr = audioEngine->GetDeviceDetails(0, &details)))
	mErrorHandler.checkResult(hr, "IXAudio2::GetDeviceDetails");

	outSampleRate = details.OutputFormat.Format.nSamplesPerSec;
	outChannelMask = details.OutputFormat.dwChannelMask;
	outChannels = details.OutputFormat.Format.nChannels;

#endif
}

void GHMetro3DAudioMgr::retrieveChannelsAndSampleRate(IXAudio2Voice* voice, UINT32& outChannels, UINT32& outSampleRate)
{
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)

	XAUDIO2_VOICE_DETAILS details;
	voice->GetVoiceDetails(&details);

	outSampleRate = details.InputSampleRate;
	outChannels = details.InputChannels;

#else

	assert(false);

#endif;
}

