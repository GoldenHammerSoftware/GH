// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundHandle.h"
#include "GHOALInclude.h"
#include "GHSoundTypes.h"

class GHOALSoundHandleMgr;

class GHOALSoundHandle : public GHSoundHandle
{
public:
	GHOALSoundHandle(GHOALSoundHandleMgr& handleMgr, 
					  ALenum format, ALsizei size,
					  ALvoid* data, ALsizei freq);
	~GHOALSoundHandle(void);

	ALuint getBufferId(void) const { return mBufferId; }
    
    virtual int getSoundHandleType(void) { return GHSoundType_OAL; }
    
    virtual GHByteStream* getByteStream(void) { return 0; }
	virtual size_t getDataSize(void) const { return 0; }
	virtual unsigned short getNumChannels(void) const { return 0; }
	virtual unsigned short getSamplesPerSecond(void) const { return 0; }
	virtual unsigned short getBitsPerSample(void) const { return 0; }
	
protected:
	GHOALSoundHandleMgr& mHandleMgr;
	ALuint mBufferId;
};
