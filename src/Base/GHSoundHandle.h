// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResource.h"
#include "GHSoundIDInfo.h"

class GHByteStream;

// interface for a sound that can be played through an emitter
class GHSoundHandle : public GHResource
{
public:
	virtual ~GHSoundHandle(void) {}

	virtual int getSoundHandleType(void) { return -1; }

	// get the sound data if possible.
	virtual GHByteStream* getByteStream(void) = 0;
	virtual size_t getDataSize(void) const = 0;
	virtual unsigned short getNumChannels(void) const = 0;
	virtual unsigned short getSamplesPerSecond(void) const = 0;
	virtual unsigned short getBitsPerSample(void) const = 0;

	float getSoundLengthInSeconds(void) const { 
		return 8.0f * (float)getDataSize() / ((float)getNumChannels() * (float)getBitsPerSample() * getSamplesPerSecond()); 
	}

public:
	GHSoundIDInfo mIDInfo;
};
