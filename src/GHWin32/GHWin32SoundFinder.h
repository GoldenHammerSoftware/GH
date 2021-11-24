// Copyright Golden Hammer Software
#pragma once

#include "GHMetro/GHXAudio2SoundFinder.h"
#include "GHWin32FileFinder.h"

class GHFileOpener;

// figures out where sounds should live on win32
class GHWin32SoundFinder : public GHXAudio2SoundFinder
{
public:
	GHWin32SoundFinder(const GHFileOpener& fileOpener);

	virtual bool createSoundReader(const char* filename, const GHPropertyContainer* extraData, 
		Microsoft::WRL::ComPtr<IMFSourceReader>& ret, GHSoundIDInfo& soundID) const;

private:
	void readSoundID(const char* filename, GHSoundIDInfo& soundID) const;
	void readID3v1(const char* buffer, size_t bufferSize, GHSoundIDInfo& soundID) const;
	void readID3v2(const char* buffer, size_t bufferSize, GHSoundIDInfo& soundID, const char* filename) const;
	void readM4AHeader(const char* buffer, size_t bufferSize, GHSoundIDInfo& soundID) const;

private:
	const GHFileOpener& mFileOpener;
	GHWin32FileFinder mFileFinder;
};
