// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"

#include "GHXAudio2Include.h"
#include <wrl.h>
#include <wrl/client.h>

class GHMetroSoundDevice;
class GHXAudio2SoundFinder;
class GHThreadFactory;
class GHMutex;

class GHMetroSoundLoader : public GHResourceLoader
{
public:
	GHMetroSoundLoader(const GHXAudio2SoundFinder& soundFinder, const GHThreadFactory& threadFactory);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);

private:
	bool getNextBuffer(byte* buffer, unsigned int maxBufferSize, unsigned int& bufferLength);

private:
	Microsoft::WRL::ComPtr<IMFSourceReader> mSoundReader;
	const GHXAudio2SoundFinder& mSoundFinder;
	GHMutex* mMutex;
};