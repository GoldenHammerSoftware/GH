// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundHandle.h"
#include "GHSoundTypes.h"
#include <AVFoundation/AVFoundation.h>

class GHMessageHandler;

// AVPlayer implementation.
class GHAVPSoundHandle : public GHSoundHandle
{
public:
    GHAVPSoundHandle(NSURL* url);
    ~GHAVPSoundHandle(void);
    
    AVPlayer* getAudioPlayer(void) { return mAudioPlayer; }
    const AVPlayer* getAudioPlayer(void) const { return mAudioPlayer; }
    
    virtual int getSoundHandleType(void) { return GHSoundType_AVPLAYER; }
    
    static void setMessageHandler(GHMessageHandler* messageHandler);
    
    virtual GHByteStream* getByteStream(void);
    virtual size_t getDataSize(void) const;
    virtual unsigned short getNumChannels(void) const;
    virtual unsigned short getSamplesPerSecond(void) const;
    virtual unsigned short getBitsPerSample(void) const;
    
    AVURLAsset* getAsset(void) { return mAsset; }
    AVPlayerItem* getPlayerItem(void) { return mPlayerItem; }
    
protected:
    AVURLAsset* mAsset;
    AVPlayerItem* mPlayerItem;
    AVPlayer* mAudioPlayer;
    
    // optional data handle.
    // mutable because of lazy evaluation.
    mutable char* mData;
    mutable GHByteStream* mByteStream;
};
