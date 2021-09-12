// Copyright 2010 Golden Hammer Software
#include "GHAVPByteStreamDecoding.h"
#include "GHPlatform/GHDebugMessage.h"
#include <stdio.h>

GHAVPByteStreamDecoding::GHAVPByteStreamDecoding(AVURLAsset& asset,
                                                 unsigned short numChannels,
                                                 unsigned short bitsPerSample,
                                                 unsigned short samplesPerSecond)
: mAsset(asset)
, mTrack(0)
, mReader(0)
, mReaderOutput(0)
, mStagingContains(0)
, mStagingPos(0)
, mStagingBuffer(0)
, mNumChannels(numChannels)
, mBitsPerSample(bitsPerSample)
, mSamplesPerSecond(samplesPerSecond)
, mInitialized(false)
, mBlockBuffer(NULL)
, mSample(NULL)
{
}

GHAVPByteStreamDecoding::~GHAVPByteStreamDecoding(void)
{
}

bool GHAVPByteStreamDecoding::readNext(char*& dest, size_t destLen, size_t& sizeRead)
{
    if (!mInitialized)
    {
        initReader();
    }
    
    unsigned int amountCopied = 0;
    while (amountCopied < destLen)
    {
        // if we need more decoded data, ask for it.
        if (mStagingPos >= mStagingContains)
        {
            if (!fillStagingBuffer())
            {
                // ran out of buffer.
                return false;
            }
        }
        // copy decoded data into the ret buf.
        size_t amountInBuffer = mStagingContains - mStagingPos;
        size_t amountToCopy = destLen;
        if (destLen > amountInBuffer)
        {
            amountToCopy = amountInBuffer;
        }
        ::memcpy(dest, &mStagingBuffer[mStagingPos], amountToCopy);
        amountCopied += amountToCopy;
        mStagingPos += amountToCopy;
    }
    sizeRead = amountCopied;
    return true;
}

void GHAVPByteStreamDecoding::reset(void)
{
    initReader();
}

void GHAVPByteStreamDecoding::initReader(void)
{
    NSError* error;
    mReader = [[AVAssetReader alloc] initWithAsset:&mAsset error:&error];
    if (error != 0)
    {
        GHDebugMessage::outputString("error creating AVAssetReader");
        return;
    }
    mTrack = [[&mAsset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0];
    
    /*
    // test hack
    CMFormatDescriptionRef formDesc = (__bridge CMFormatDescriptionRef)[[mTrack formatDescriptions] objectAtIndex:0];
    const AudioStreamBasicDescription* asbdPointer = CMAudioFormatDescriptionGetStreamBasicDescription(formDesc);
    //because this is a pointer and not a struct we need to move the data into a struct so we can use it
    AudioStreamBasicDescription asbd = {0};
    memcpy(&asbd, asbdPointer, sizeof(asbd));
    */
    
    /*
    AudioChannelLayout channelLayout;
    memset(&channelLayout, 0, sizeof(AudioChannelLayout));
    channelLayout.mChannelLayoutTag = kAudioChannelLayoutTag_Mono;
    
    NSMutableDictionary* audioReadSettings = [NSMutableDictionary dictionary];
    // keys within linear pcm format settings
    [audioReadSettings setValue:[NSNumber numberWithInt:kAudioFormatLinearPCM] forKey:AVFormatIDKey];
    [audioReadSettings setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsFloatKey];
    [audioReadSettings setValue:[NSNumber numberWithInt:mBitsPerSample] forKey:AVLinearPCMBitDepthKey];
    [audioReadSettings setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsNonInterleaved];
    [audioReadSettings setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsBigEndianKey];
    
    // keys not found in linear pcm format settings
    [audioReadSettings setValue:[NSNumber numberWithInt:mNumChannels] forKey:AVNumberOfChannelsKey];
    [audioReadSettings setValue:[NSNumber numberWithFloat:mSamplesPerSecond] forKey:AVSampleRateKey];
    [audioReadSettings setValue:[NSData dataWithBytes:&channelLayout length:sizeof(AudioChannelLayout)] forKey:AVChannelLayoutKey];
    */
    
    NSDictionary *audioReadSettings =
    [NSDictionary dictionaryWithObjectsAndKeys:
     [NSNumber numberWithInt:kAudioFormatLinearPCM], AVFormatIDKey,
     [NSNumber numberWithFloat:mSamplesPerSecond], AVSampleRateKey,
     [NSNumber numberWithInt:mBitsPerSample], AVLinearPCMBitDepthKey,
     [NSNumber numberWithBool:NO], AVLinearPCMIsNonInterleaved,
     [NSNumber numberWithBool:NO],AVLinearPCMIsFloatKey,
     [NSNumber numberWithBool:NO], AVLinearPCMIsBigEndianKey,
     [NSNumber numberWithInt:mNumChannels], AVNumberOfChannelsKey,
     nil];
    
    mReaderOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:mTrack outputSettings:audioReadSettings];
    [mReader addOutput:mReaderOutput];
    [mReader startReading];
    mInitialized = true;
}

bool GHAVPByteStreamDecoding::fillStagingBuffer(void)
{
    mSample = [mReaderOutput copyNextSampleBuffer];
    if (mSample == NULL)
    {
        return false;
    }

    mStagingPos = 0;
    mBlockBuffer = CMSampleBufferGetDataBuffer(mSample);
    size_t lengthAtOffset;
    OSStatus bufStatus = CMBlockBufferGetDataPointer(mBlockBuffer, 0, &lengthAtOffset, &mStagingContains, &mStagingBuffer);
    if (bufStatus != noErr)
    {
        GHDebugMessage::outputString("Failed CMBlockBufferGetDataPointer");
        return false;
    }
    return true;
}
