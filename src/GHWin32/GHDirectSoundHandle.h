// Copyright Golden Hammer Software
#pragma once

#include "Base/GHSoundHandle.h"
#include "GHPlatform/win32/GHWin32Include.h"
#include <dsound.h>

class GHDirectSoundHandle : public GHSoundHandle
{
public:
	GHDirectSoundHandle(IDirectSoundBuffer8* soundBuffer);
	~GHDirectSoundHandle(void);

	IDirectSoundBuffer8* getBuffer(void) { return mSoundBuffer; }

	virtual GHByteStream* getByteStream(void) { return 0; }
	size_t getDataSize(void) const { return 0; }
	virtual unsigned short getNumChannels(void) const { return 0; }
	virtual unsigned short getSamplesPerSecond(void) const { return 0; }
	virtual unsigned short getBitsPerSample(void) const { return 0; }

private:
	IDirectSoundBuffer8* mSoundBuffer;
};
