// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/win32/GHWin32Include.h"
#include <dsound.h>
#include "GHUtils/GHResourceLoader.h"

class GHFileOpener;
class GHDirectSoundDevice;

// http://www.rastertek.com/dx10tut14.html
class GHDirectSoundLoader : public GHResourceLoader
{
public:
	GHDirectSoundLoader(const GHFileOpener& fileOpener, GHDirectSoundDevice& device);

	// returns a GHDirectSoundHandle
	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData = 0);

private:
	struct WaveHeader
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};
	bool verifyHeader(const WaveHeader& header);
	IDirectSoundBuffer8* createEmptyBuffer(const WaveHeader& waveHeader);

private:
	const GHFileOpener& mFileOpener;
	GHDirectSoundDevice& mDevice;
};
