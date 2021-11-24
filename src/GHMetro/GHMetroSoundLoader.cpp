// Copyright Golden Hammer Software
#include "GHMetroSoundLoader.h"
#include "GHMetroSoundHandle.h"
#include "GHPlatform/GHFile.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMetroSoundDevice.h"
#include "GHXAudio2SoundFinder.h"
#include "GHXAudio2Include.h"
#include "Base/GHSoundIDInfo.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHMutex.h"

//Wave file reading method mostly copied from
// Microsoft's MarbleMaze sample

GHMetroSoundLoader::GHMetroSoundLoader(const GHXAudio2SoundFinder& soundFinder, const GHThreadFactory& threadFactory)
: mSoundFinder(soundFinder)
, mMutex(threadFactory.createMutex())
{
	
}

GHResource* GHMetroSoundLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	mMutex->acquire();

	GHSoundIDInfo soundID;
	if (!mSoundFinder.createSoundReader(filename, extraData, mSoundReader, soundID))
	{
		GHDebugMessage::outputString("Failed to create reader for %s", filename);
		mMutex->release();
		return 0;
	}

	Microsoft::WRL::ComPtr<IMFMediaType> mediaType;
	HRESULT result = MFCreateMediaType(&mediaType);
	if (FAILED(result))
	{
		GHDebugMessage::outputString("Function MFCreateMediaType failed in sound loading with result 0x%X", result);
		mMutex->release();
		return 0;
	}

	result = mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	result = mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

	result = mSoundReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, mediaType.Get());

	Microsoft::WRL::ComPtr<IMFMediaType> outputMediaType;
    result = mSoundReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &outputMediaType);

	unsigned int formatSize = 0;
	WAVEFORMATEX* waveFormat;
	result = MFCreateWaveFormatExFromMFMediaType(outputMediaType.Get(), &waveFormat, &formatSize);
	if (waveFormat->wFormatTag == WAVE_FORMAT_PCM) {
		GHDebugMessage::outputString("PCM");
	}

	PROPVARIANT var;
	result = mSoundReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var);

	ULONGLONG duration = var.uhVal.QuadPart;
	unsigned int bufferSize = static_cast<unsigned int>((duration*static_cast<ULONGLONG>(waveFormat->nAvgBytesPerSec) + 10000000) / 10000000);
	bufferSize += 4 - (bufferSize % 4);

	byte* playData = new byte[bufferSize];

	GHMetroSoundHandle* ret = new GHMetroSoundHandle(playData, bufferSize, *waveFormat);
	ret->mIDInfo = soundID;
	CoTaskMemFree(waveFormat);

	unsigned char* bufferPtr = playData;
	unsigned int bytesWritten = 0;
	while (!getNextBuffer(bufferPtr, bufferSize, bytesWritten))
	{
		bufferPtr += bytesWritten;
		if (bufferSize <= bytesWritten) {
			bufferSize = 0;
			break;
		}
		bufferSize -= bytesWritten;
	}
	if (bufferSize > 0)
	{
		// make sure we don't have any trailing garbage at the end of the file.
		memset(bufferPtr, 0, bufferSize);
	}
//	ret->mSourceVoice = mSoundDevice.createSourceVoice(ret->mFormat);

	mMutex->release();
	return ret;
}

bool GHMetroSoundLoader::getNextBuffer(byte* buffer, unsigned int maxBufferSize, unsigned int& bufferLength)
{
	HRESULT result = S_OK;
	Microsoft::WRL::ComPtr<IMFSample> sample;
	DWORD flags = 0;
	result = mSoundReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &flags, nullptr, &sample);
	if (!SUCCEEDED(result))
	{
		return true;
	}

	if (sample == nullptr)
	{
		return (flags & MF_SOURCE_READERF_ENDOFSTREAM) != 0;
	}

	Microsoft::WRL::ComPtr<IMFMediaBuffer> mediaBuffer;
	result = sample->ConvertToContiguousBuffer(&mediaBuffer);

	BYTE* audioData = nullptr;
	DWORD sampleBufferLength = 0;
	result = mediaBuffer->Lock(&audioData, nullptr, &sampleBufferLength);

	if (sampleBufferLength > maxBufferSize)
	{
		// the song is not evenly divisible by our sample size, so this is the last entry.
		if (maxBufferSize > 1) {
			CopyMemory(buffer, audioData, maxBufferSize - 1);
		}
		return false;
	}

	CopyMemory(buffer, audioData, sampleBufferLength);
	bufferLength = sampleBufferLength;

	return (flags & MF_SOURCE_READERF_ENDOFSTREAM) != 0;
}
