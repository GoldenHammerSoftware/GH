// Copyright Golden Hammer Software
#include "GHDirectSoundDevice.h"
#include "GHDirectSoundEmitter.h"
#include "GHDirectSoundHandle.h"
#include "GHWin32Window.h"
#include "GHPlatform/GHDebugMessage.h"

GHDirectSoundDevice::GHDirectSoundDevice(GHSoundVolumeMgr& volumeMgr, GHWin32Window& window)
: mVolumeMgr(volumeMgr)
, mDirectSound(0)
, mPrimaryBuffer(0)
{
	init(window);
}

GHDirectSoundDevice::~GHDirectSoundDevice(void)
{
	shutdown();
}

bool GHDirectSoundDevice::init(GHWin32Window& window)
{
	// reference: http://www.rastertek.com/dx10tut14.html

	HRESULT result;

	// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &mDirectSound, NULL);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("Failed DirectSoundCreate");
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = mDirectSound->SetCooperativeLevel(window.getHWND(), DSSCL_PRIORITY);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("Failed SetCooperativeLevel");
		return false;
	}

	if (!initPrimaryBuffer())
	{
		return false;
	}

	return true;
}

bool GHDirectSoundDevice::initPrimaryBuffer(void)
{
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;
	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	HRESULT result = mDirectSound->CreateSoundBuffer(&bufferDesc, &mPrimaryBuffer, NULL);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("Failed to create primary sound buffer");
		return false;
	}

	// Setup the format of the primary sound bufffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	result = mPrimaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("Failed to set primary sound buffer format");
		return false;
	}

	return true;
}

void GHDirectSoundDevice::shutdown(void)
{
	if (mPrimaryBuffer)
	{
		mPrimaryBuffer->Release();
		mPrimaryBuffer = 0;
	}
	if (mDirectSound)
	{
		mDirectSound->Release();
		mDirectSound = 0;
	}
}

void GHDirectSoundDevice::enable3DSound(bool enable)
{
	// todo
}

void GHDirectSoundDevice::setListenerPosition(const GHPoint3& pos)
{
	// todo?
}

void GHDirectSoundDevice::setListenerOrientation(const GHPoint3& dir)
{
	// todo 
}

void GHDirectSoundDevice::setListenerVelocity(const GHPoint3& velocity)
{
	// todo
}

GHSoundEmitter* GHDirectSoundDevice::createEmitter(GHSoundHandle& sound, const GHIdentifier& category)
{
	GHDirectSoundHandle& dsHandle = *((GHDirectSoundHandle*)&sound);
	return new GHDirectSoundEmitter(dsHandle, category, mVolumeMgr);
}

