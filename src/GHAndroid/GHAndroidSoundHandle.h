// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundHandle.h"
#include <jni.h>
#include <assert.h>

class GHAndroidSoundPool;

class GHAndroidSoundHandle : public GHSoundHandle
{
public:
	GHAndroidSoundHandle(GHAndroidSoundPool& mediaPlayer, jobject mediaPlayerObj);
	~GHAndroidSoundHandle(void);
    jobject mMediaPlayerObj;
    
    // get the sound data if possible.
	virtual GHByteStream* getByteStream(void) { assert(false); return 0; }
	virtual size_t getDataSize(void) const { assert(false); return 0; }
	virtual unsigned short getNumChannels(void) const { assert(false); return 0; }
	virtual unsigned short getSamplesPerSecond(void) const { assert(false); return 0; }
	virtual unsigned short getBitsPerSample(void) const { assert(false); return 0; }
    
private:
    GHAndroidSoundPool& mMediaPlayer;
};
