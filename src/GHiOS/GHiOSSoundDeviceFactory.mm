// Copyright 2010 Golden Hammer Software
#include "GHiOSSoundDeviceFactory.h"
#include "GHiOSSoundDevice.h"
#include <AudioToolbox/AudioServices.h>

GHiOSSoundDeviceFactory::GHiOSSoundDeviceFactory(GHEventMgr& eventMgr, GHSoundVolumeMgr& volumeMgr)
: mEventMgr(eventMgr)
, mVolumeMgr(volumeMgr)
{
}

GHiOSSoundDevice* GHiOSSoundDeviceFactory::createDevice(void)
{
    // don't interrupt itunes.
	UInt32 isPlaying;
	UInt32 propertySize = sizeof(isPlaying);
	AudioSessionInitialize(NULL, NULL, NULL, NULL);
	AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &propertySize, &isPlaying);	
	UInt32 sessionCategory = kAudioSessionCategory_AmbientSound;
	AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(sessionCategory), &sessionCategory);

    
    return new GHiOSSoundDevice(isPlaying, mEventMgr, mVolumeMgr);
}
