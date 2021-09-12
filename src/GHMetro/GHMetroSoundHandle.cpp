// Copyright Golden Hammer Software
#include "GHMetroSoundHandle.h"
#include "GHPlatform/GHByteStreamPreallocated.h"

GHMetroSoundHandle::GHMetroSoundHandle(byte* rawData, size_t dataSize, const WAVEFORMATEX& format)
: mPlayData(rawData)
, mPlayDataSize(dataSize)
{
	CopyMemory(&mFormat, &format, sizeof(mFormat));
	mByteStream = new GHByteStreamPreallocated((char*)mPlayData, mPlayDataSize);
}

GHMetroSoundHandle::~GHMetroSoundHandle(void)
{
	delete mByteStream;
	delete [] mPlayData;
}
