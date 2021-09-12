// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXAudio2SoundFinder.h"

class GHMetroFileOpener;

// figures out where sounds should live on metro
class GHMetroSoundFinder : public GHXAudio2SoundFinder
{
public:
	GHMetroSoundFinder(const GHMetroFileOpener& fileOpener);

	virtual bool createSoundReader(const char* filename, const GHPropertyContainer* extraData, 
		Microsoft::WRL::ComPtr<IMFSourceReader>& ret, GHSoundIDInfo& retInfo) const;

private:
	void collectSoundID(Windows::Storage::StorageFile^ storageFile, GHSoundIDInfo& retInfo) const;

private:
	const GHMetroFileOpener& mFileOpener;
};
