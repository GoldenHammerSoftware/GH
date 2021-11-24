// Copyright Golden Hammer Software
#pragma once

#include "GHXAudio2Include.h"
#include <wrl.h>
#include <wrl/client.h>
#include "Base/GHSoundIDInfo.h"

class GHPropertyContainer;

// interface for finding out where a sound file lives.
class GHXAudio2SoundFinder
{
public:
	virtual bool createSoundReader(const char* filename, const GHPropertyContainer* extraData, 
		Microsoft::WRL::ComPtr<IMFSourceReader>& ret, GHSoundIDInfo& retInfo) const = 0;
};
