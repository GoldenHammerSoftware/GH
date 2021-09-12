// Copyright Golden Hammer Software
#include "GHDirectSoundHandle.h"
#include <assert.h>

GHDirectSoundHandle::GHDirectSoundHandle(IDirectSoundBuffer8* soundBuffer)
: mSoundBuffer(soundBuffer)
{
	assert(mSoundBuffer);
}

GHDirectSoundHandle::~GHDirectSoundHandle(void)
{
	if (mSoundBuffer)
	{
		mSoundBuffer->Release();
		mSoundBuffer = 0;
	}
}
