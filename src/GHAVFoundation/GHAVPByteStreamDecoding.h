// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHByteStream.h"
#include <AVFoundation/AVFoundation.h>

// a byte stream that decodes a music file as new chunks are requested.
class GHAVPByteStreamDecoding : public GHByteStream
{
public:
    GHAVPByteStreamDecoding(AVURLAsset& asset, unsigned short numChannels, unsigned short bitsPerSample, unsigned short samplesPerSecond);
    ~GHAVPByteStreamDecoding(void);
    
    virtual bool readNext(char*& dest, size_t destLen, size_t& sizeRead);
    virtual void reset(void);
    
private:
    void initReader(void);
    // decode some bytes into the staging buffer.
    bool fillStagingBuffer(void);
    
private:
    AVURLAsset& mAsset;
    unsigned short mNumChannels;
    unsigned short mBitsPerSample;
    unsigned short mSamplesPerSecond;
    
    AVAssetReader* mReader;
    AVAssetTrack* mTrack;
    AVAssetReaderTrackOutput* mReaderOutput;
    
    // the sample that contains mBlockBuffer
    CMSampleBufferRef mSample;
    // buffer that holds the staging buffer memory.
    CMBlockBufferRef mBlockBuffer;
    // a staging buffer that we decode into before filling in readNext
    // this is not allocated memory.  it's a pointer into mBlockBuffer.
    char* mStagingBuffer;
    
    // position of the next decoded byte in staging buffer.
    size_t mStagingPos;
    // number of decoded bytes within the staging buffer.
    // we don't realloc staging buffer unless it is not big enough.
    size_t mStagingContains;
    // has initReader been called yet?
    bool mInitialized;
};
