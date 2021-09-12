// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundHandle.h"
#include "GHSoundTypes.h"
#include <AVFoundation/AVFoundation.h>

class GHMessageHandler;

class GHAVFSoundHandle : public GHSoundHandle
{
public:
	GHAVFSoundHandle(NSURL* url);
	~GHAVFSoundHandle(void);
	
	AVAudioPlayer* getAudioPlayer(void) { return mAudioPlayer; }
	const AVAudioPlayer* getAudioPlayer(void) const { return mAudioPlayer; }
	
	virtual int getSoundHandleType(void) { return GHSoundType_AVF; }
	
	static void setMessageHandler(GHMessageHandler* messageHandler);
    
    virtual GHByteStream* getByteStream(void) { return 0; }
	virtual size_t getDataSize(void) const { return 0; }
	virtual unsigned short getNumChannels(void) const { return 0; }
	virtual unsigned short getSamplesPerSecond(void) const { return 0; }
	virtual unsigned short getBitsPerSample(void) const { return 0; }
	
protected:
	AVAudioPlayer* mAudioPlayer;
};
