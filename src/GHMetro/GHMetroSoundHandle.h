// Copyright Golden Hammer Software
#pragma once

#include "Base/GHSoundHandle.h"
#include "GHXAudio2Include.h"
#include "GHString/GHString.h"

class GHMetroSoundHandle : public GHSoundHandle
{
public:
	GHMetroSoundHandle(byte* rawData, size_t dataSize, const WAVEFORMATEX& format);
	~GHMetroSoundHandle(void);

	virtual GHByteStream* getByteStream(void) { return mByteStream; }
	size_t getDataSize(void) const { return mPlayDataSize; }
	virtual unsigned short getNumChannels(void) const { return mFormat.nChannels; }
	virtual unsigned short getSamplesPerSecond(void) const { return (unsigned short)mFormat.nSamplesPerSec; }
	virtual unsigned short getBitsPerSample(void) const { return (unsigned short)mFormat.wBitsPerSample; }

	const byte* getRawData(void) const { return mPlayData; }
	const WAVEFORMATEX& getFormat(void) const { return mFormat; }

private:
	GHByteStream* mByteStream;
	byte* mPlayData;
	size_t mPlayDataSize;
	WAVEFORMATEX mFormat;
};