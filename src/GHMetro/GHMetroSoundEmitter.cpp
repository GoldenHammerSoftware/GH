// Copyright Golden Hammer Software
#include "GHMetroSoundEmitter.h"
#include "GHMetroSoundHandle.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHMetroSoundDevice.h"

GHMetroSoundEmitter::GHMetroSoundEmitter(GHMetroSoundDevice& soundDevice,
										 GHMetroSoundHandle& soundHandle,
										 const GHIdentifier& category,
										 GHSoundVolumeMgr& volumeMgr)
	: GHSoundEmitter(category, volumeMgr) 
	, mSoundDevice(soundDevice)
	, mSoundHandle(soundHandle)
	, mIsPositional(false)
	, mIsPlaying(false)
	, mCallback(mIsPlaying)
	, mCurrentGain(1.f)
	, mIsPaused(false)
{
	mSoundHandle.acquire();

	ZeroMemory(&m3DEmitter, sizeof(m3DEmitter));

	m3DEmitter.pCone = &m3DCone;
	m3DEmitter.pCone->InnerAngle = X3DAUDIO_2PI;
	// Setting the inner cone angles to X3DAUDIO_2PI and
	// outer cone other than 0 causes
	// the emitter to act like a point emitter using the
	// INNER cone settings only.
	m3DEmitter.pCone->OuterAngle = 0.0f;
	// Setting the outer cone angles to zero causes
	// the emitter to act like a point emitter using the
	// OUTER cone settings only.
	m3DEmitter.pCone->InnerVolume = 0.0f;
	m3DEmitter.pCone->OuterVolume = 1.0f;
	m3DEmitter.pCone->InnerLPF = 0.0f;
	m3DEmitter.pCone->OuterLPF = 1.0f;
	m3DEmitter.pCone->InnerReverb = 0.0f;
	m3DEmitter.pCone->OuterReverb = 1.0f;

	m3DEmitter.InnerRadius = 2.0f;
	m3DEmitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;
	m3DEmitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
	m3DEmitter.pLFECurve = nullptr;
	m3DEmitter.pLPFDirectCurve = nullptr;
	m3DEmitter.pLPFReverbCurve = nullptr;
	m3DEmitter.pReverbCurve = nullptr;
	m3DEmitter.CurveDistanceScaler = 14.0f;
	m3DEmitter.DopplerScaler = 1.0f;

	init();

	setLooping(false);
	mSoundDevice.registerEmitter(this);
	updateGain();
}

GHMetroSoundEmitter::~GHMetroSoundEmitter(void)
{
	mSoundDevice.unregisterEmitter(this);
	shutdown();
	mSoundHandle.release();
	delete[] m3DEmitter.pChannelAzimuths;
}

void GHMetroSoundEmitter::init(void)
{
	mSourceVoice = mSoundDevice.createSourceVoice(mSoundHandle, &mCallback);

	delete[] m3DEmitter.pChannelAzimuths;
	m3DEmitter.pChannelAzimuths = nullptr;

	m3DEmitter.ChannelCount = mSoundHandle.getNumChannels();
	if (m3DEmitter.ChannelCount > 1)
	{
		m3DEmitter.pChannelAzimuths = new FLOAT32[m3DEmitter.ChannelCount];
		ZeroMemory(m3DEmitter.pChannelAzimuths, sizeof(FLOAT32)*m3DEmitter.ChannelCount);
	}

	setGain(mCurrentGain);

	if (mIsPlaying)
	{
		play();
	}
}

void GHMetroSoundEmitter::shutdown(void)
{
	if (mSourceVoice)
	{
		mSourceVoice->DestroyVoice();
	}
	mSourceVoice = 0;
}

void GHMetroSoundEmitter::play(void)
{
	mIsPlaying = true;
	if (!mSourceVoice) { return; }

	if (!mIsPaused)
	{
		mSourceVoice->FlushSourceBuffers();
		mSourceVoice->SubmitSourceBuffer(&mPlayBuffer);
	}
	mIsPaused = false;

	HRESULT result = mSourceVoice->Start();
	if (FAILED(result))
	{
		GHDebugMessage::outputString("IXAudio2SourceVoice::Start failed with result 0x%X", result);
	}
}

void GHMetroSoundEmitter::pause(void)
{
	mIsPlaying = false;
	stop();
	mIsPaused = true;
}

void GHMetroSoundEmitter::stop(void)
{
	mIsPlaying = false;
	mIsPaused = false;
	if (!mSourceVoice) { return; }

	HRESULT result = mSourceVoice->Stop();
	if (FAILED(result))
	{
		GHDebugMessage::outputString("IXAudio2SourceVoice::Stop failed with result 0x%X", result);
	}
}

void GHMetroSoundEmitter::setIsPositional(bool isPositional)
{
	mIsPositional = isPositional;
}

//X3DAudio uses a left - handed Cartesian coordinate system, with values
//	on the x - axis increasing from left to right,
//	on the y - axis from bottom to top, and 
//	on the z - axis from near to far.
void GHMetroSoundEmitter::setPosition(const GHPoint3& pos)
{
	m3DEmitter.Position.x = pos[0];
	m3DEmitter.Position.y = pos[1];
	m3DEmitter.Position.z = pos[2];
}

void GHMetroSoundEmitter::setVelocity(const GHPoint3& pos)
{
	m3DEmitter.Velocity.x = pos[0];
	m3DEmitter.Velocity.y = pos[1];
	m3DEmitter.Velocity.z = pos[2];
}

void GHMetroSoundEmitter::setLooping(bool looping)
{
	if (!mSourceVoice) {
		return;
	}
	ZeroMemory(&mPlayBuffer, sizeof(mPlayBuffer));
	mPlayBuffer.AudioBytes = (UINT32)mSoundHandle.getDataSize();
	mPlayBuffer.pAudioData = mSoundHandle.getRawData();
    mPlayBuffer.Flags = XAUDIO2_END_OF_STREAM; 
	mPlayBuffer.LoopCount = looping ? XAUDIO2_LOOP_INFINITE : 0;
	mSourceVoice->FlushSourceBuffers();
	mSourceVoice->SubmitSourceBuffer(&mPlayBuffer);
}

void GHMetroSoundEmitter::setPitch(float pitch)
{
	if (!mSourceVoice) {
		return;
	}

	//Not sure exactly how to convert pitch to "frequency ratio"
	// the function call here should be mSourceVoice->setFrequencyRatio()
	// with some value based on pitch. 
	
	mSourceVoice->SetFrequencyRatio(pitch);
}

void GHMetroSoundEmitter::applyGain(float gain)
{
	mCurrentGain = gain;
	if (mSourceVoice)
	{
		mSourceVoice->SetVolume(mCurrentGain);
	}
}

bool GHMetroSoundEmitter::isPositional(void) const
{
	return mIsPositional;
}

bool GHMetroSoundEmitter::isPlaying(void) const
{
	return mIsPlaying;
}

bool GHMetroSoundEmitter::isLooping(void) const
{
	return mPlayBuffer.LoopCount != 0;
}

void STDMETHODCALLTYPE GHMetroSoundEmitter::SoundCallback::OnVoiceError(void *pBufferContext, HRESULT error)
{
	GHDebugMessage::outputString("GHMetroSoundEmitter onVoiceError");
}

void GHMetroSoundEmitter::seek(float t)
{
	if (!mSourceVoice) {
		return;
	}

	unsigned int samplesPerSec = mSoundHandle.getSamplesPerSecond();
	unsigned int totalSamples = int(float(samplesPerSec) * mSoundHandle.getSoundLengthInSeconds());
	// my dying bride: deeper down was crashing
	if (totalSamples > 0) totalSamples -= 1;

	unsigned int loopCount = mPlayBuffer.LoopCount;

	ZeroMemory(&mPlayBuffer, sizeof(mPlayBuffer));
	mPlayBuffer.AudioBytes = (UINT32)mSoundHandle.getDataSize();
	mPlayBuffer.pAudioData = mSoundHandle.getRawData();
	mPlayBuffer.Flags = XAUDIO2_END_OF_STREAM;
	mPlayBuffer.PlayBegin = int((t * float(samplesPerSec)));
	mPlayBuffer.PlayLength = totalSamples - mPlayBuffer.PlayBegin;
	mPlayBuffer.LoopCount = loopCount;

	mSourceVoice->FlushSourceBuffers();
	mSourceVoice->SubmitSourceBuffer(&mPlayBuffer);
}
