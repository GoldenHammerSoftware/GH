// Copyright Golden Hammer Software
#pragma once

#include "GHXAudio2SoundFinder.h"

class GHFileOpener;

// figures out where sounds should live on win32
class GHWin32SoundFinder : public GHXAudio2SoundFinder
{
public:
	GHWin32SoundFinder(const GHFileOpener& fileOpener);

	virtual bool createSoundReader(const char* filename, const GHPropertyContainer* extraData, 
		Microsoft::WRL::ComPtr<IMFSourceReader>& ret, GHSoundIDInfo& soundID) const;

private:
	virtual bool createFilePath(const char* filename, wchar_t* ret, size_t retSize) const;

	void readSoundID(const char* filename, GHSoundIDInfo& soundID) const;
	void readID3v1(const char* buffer, size_t bufferSize, GHSoundIDInfo& soundID) const;
	void readID3v2(const char* buffer, size_t bufferSize, GHSoundIDInfo& soundID, const char* filename) const;
	void readM4AHeader(const char* buffer, size_t bufferSize, GHSoundIDInfo& soundID) const;

private:
	const GHFileOpener& mFileOpener;
};
