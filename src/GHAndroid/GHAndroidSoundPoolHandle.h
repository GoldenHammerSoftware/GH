// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundHandle.h"
#include "GHString/GHString.h"
#include <assert.h>

// a define to pretend we have access to sound data.
#define FAKE_SOUND_DATA

#ifdef FAKE_SOUND_DATA
class GHAndroidSoundPoolHandle : public GHSoundHandle
{
private:
	static const int NUMSECONDS = 12;
	static const int SAMPLESPERSECOND = 22000;
	static const int BITSPERSAMPLE = 8;

public:
	GHAndroidSoundPoolHandle(const char* filename);
	~GHAndroidSoundPoolHandle(void);

	virtual GHByteStream* getByteStream(void);
	virtual size_t getDataSize(void) const { return NUMSECONDS*SAMPLESPERSECOND*(BITSPERSAMPLE / 8); }
	virtual unsigned short getNumChannels(void) const { return 1; }
	virtual unsigned short getSamplesPerSecond(void) const { return SAMPLESPERSECOND; }
	virtual unsigned short getBitsPerSample(void) const { return BITSPERSAMPLE; }

public:
	GHString mFilename;
	// fake sound buf
	char mSoundBuf[NUMSECONDS*SAMPLESPERSECOND*(BITSPERSAMPLE / 8)];
};

#else
class GHAndroidSoundPoolHandle : public GHSoundHandle
{
public:
	GHAndroidSoundPoolHandle(const char* filename);
	~GHAndroidSoundPoolHandle(void);
    
	virtual GHByteStream* getByteStream(void);
	virtual size_t getDataSize(void) const { return 0; }
	virtual unsigned short getNumChannels(void) const { return 0; }
	virtual unsigned short getSamplesPerSecond(void) const { return 0; }
	virtual unsigned short getBitsPerSample(void) const { return 0; }

public:
    GHString mFilename;
};
#endif