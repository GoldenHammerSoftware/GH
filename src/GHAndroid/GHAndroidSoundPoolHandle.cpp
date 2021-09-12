// Copyright 2010 Golden Hammer Software
#include "GHAndroidSoundPoolHandle.h"
#include "GHPlatform/GHByteStreamPreallocated.h"

GHAndroidSoundPoolHandle::GHAndroidSoundPoolHandle(const char* filename)
{
    mFilename.setConstChars(filename, GHString::CHT_COPY);
}

GHAndroidSoundPoolHandle::~GHAndroidSoundPoolHandle(void)
{
}

#ifdef FAKE_SOUND_DATA
GHByteStream* GHAndroidSoundPoolHandle::getByteStream(void)
{
	return new GHByteStreamPreallocated(mSoundBuf, NUMSECONDS*SAMPLESPERSECOND*(BITSPERSAMPLE / 8));
}
#else
GHByteStream* GHAndroidSoundPoolHandle::getByteStream(void)
{
	return new GHByteStreamPreallocated(0, 0);
}
#endif