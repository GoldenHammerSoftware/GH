// Copyright 2010 Golden Hammer Software
#include "GHAppleOALSoundLoader.h"
#include "GHOALSoundHandle.h"
#include "AudioToolbox/AudioFile.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHAppleFilenameCreator.h"

GHAppleOALSoundLoader::GHAppleOALSoundLoader(GHOALSoundHandleMgr& handleMgr,
                                             GHAppleFilenameCreator* filenameCreator)
:	mHandleMgr(handleMgr)
,   mFileNameCreator(filenameCreator)
{

}

GHAppleOALSoundLoader::~GHAppleOALSoundLoader(void)
{
    delete mFileNameCreator;
}

GHResource* GHAppleOALSoundLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	NSString* path;
	if (!mFileNameCreator->createFileName(filename, path)) {
		return 0;
	}
    CFURLRef url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (UInt8*)[path UTF8String], 
														   strlen([path UTF8String]), false);
    if (!url) {
		GHDebugMessage::outputString("Failed to create url for sound file.\n");
        return 0;
	}
	
	AudioFileID audioFileID;
	OSStatus openRes = AudioFileOpenURL(url, kAudioFileReadPermission, 0, &audioFileID);
	CFRelease(url);
	if (openRes != noErr) {
		GHDebugMessage::outputString("Failed AudioFileOpenURL\n");
		return 0;
	}
	
	AudioStreamBasicDescription audioDesc;
	UInt32 descSize = sizeof(audioDesc);  
	OSStatus getPropRes = AudioFileGetProperty(audioFileID, kAudioFilePropertyDataFormat, 
											   &descSize, &audioDesc);
	if (getPropRes != noErr) {
		GHDebugMessage::outputString("Failed to get data format from audio file.\n");
		return 0;
	}
	
	UInt64 numPackets;
	descSize = sizeof(UInt64);
	getPropRes = AudioFileGetProperty(audioFileID, kAudioFilePropertyAudioDataPacketCount, 
									  &descSize, &numPackets);
	if (getPropRes != noErr) {
		GHDebugMessage::outputString("Failed to get packet count for file.\n");
		AudioFileClose(audioFileID);
		return 0;
	}
	UInt32 numBytes = numPackets * audioDesc.mBytesPerPacket;
	unsigned char* buffer = new unsigned char[numBytes];
	
	OSStatus readBytesRes = AudioFileReadBytes(audioFileID, false, 0, &numBytes, buffer);
	if (readBytesRes != noErr) {
		GHDebugMessage::outputString("Failed to read bytes from sound file.\n");
	}
	buffer[numBytes-1] = 0;
	buffer[numBytes-2] = 0;
	buffer[numBytes-3] = 0;
	buffer[numBytes-4] = 0;
	
	ALenum alFormat = AL_FORMAT_MONO8;
	if (audioDesc.mBitsPerChannel == 16) {
		if (audioDesc.mChannelsPerFrame == 2) {
			alFormat = AL_FORMAT_STEREO16;
		}
		else {
			alFormat = AL_FORMAT_MONO16;
		}
	}
	else if (audioDesc.mChannelsPerFrame == 2) {
		alFormat = AL_FORMAT_STEREO8;
	}
	GHOALSoundHandle* ret = new GHOALSoundHandle(mHandleMgr, alFormat, numBytes, buffer, 
												   audioDesc.mSampleRate);
	
	AudioFileClose(audioFileID);
	delete [] buffer;
	return ret;
}

