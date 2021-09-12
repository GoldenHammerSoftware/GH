// Copyright 2010 Golden Hammer Software
#include "GHAVPSoundHandle.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHProfiler.h"
#include "GHAVPByteStreamDecoding.h"

GHAVPSoundHandle::GHAVPSoundHandle(NSURL* url)
: mAudioPlayer(nil)
, mPlayerItem(nil)
, mAsset(nil)
, mData(0)
, mByteStream(0)
{
    mAsset = [[AVURLAsset alloc]initWithURL:url options:nil];
    if (!mAsset) {
        GHDebugMessage::outputString("Failed to load AVURLAsset");
        return;
    }
    mPlayerItem = [[AVPlayerItem alloc]initWithAsset:mAsset];
    if (!mPlayerItem)
    {
        GHDebugMessage::outputString("Failed to create AVPlayerItem");
        return;
    }
    mAudioPlayer = [[AVPlayer alloc]initWithPlayerItem:mPlayerItem];
    if (!mAudioPlayer)
    {
        GHDebugMessage::outputString("Failed to create AVPlayer");
        return;
    }
}

GHAVPSoundHandle::~GHAVPSoundHandle(void)
{
    delete [] mData;
    delete mByteStream;
}

GHByteStream* GHAVPSoundHandle::getByteStream(void)
{
    GHPROFILESCOPE("GHAVPSoundHandle::getByteStream", GHString::CHT_REFERENCE);
    
    if (mByteStream) {
        return mByteStream;
    }
    if (!mAsset)
    {
        return 0;
    }
    mByteStream = new GHAVPByteStreamDecoding(*mAsset, getNumChannels(), getBitsPerSample(), getSamplesPerSecond());
    return mByteStream;
}

size_t GHAVPSoundHandle::getDataSize(void) const
{
    float duration = CMTimeGetSeconds(mAsset.duration);
    size_t sampleSizeBytes = getNumChannels() * getBitsPerSample() / 8;
    size_t totalBufSize = duration * sampleSizeBytes * getSamplesPerSecond();
    return totalBufSize;
}

unsigned short GHAVPSoundHandle::getNumChannels(void) const
{
    // matches AVNumberOfChannelsKey inside getData()
    return 1;
}

unsigned short GHAVPSoundHandle::getSamplesPerSecond(void) const
{
    // matches AVSampleRateKey inside getData()
    return 22000;
}

unsigned short GHAVPSoundHandle::getBitsPerSample(void) const
{
    // matches AVLinearPCMBitDepthKey inside getData
    return 16;
}
