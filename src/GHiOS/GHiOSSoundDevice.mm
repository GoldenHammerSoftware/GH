// Copyright 2010 Golden Hammer Software
#include "GHiOSSoundDevice.h"
#include "GHSoundTypes.h"
#include "GHOALSoundHandle.h"
#include "GHAVFSoundHandle.h"
#include "GHAVFSoundDevice.h"
#include "GHUtils/GHEventMgr.h"
#include "GHSoundTypes.h"

GHiOSSoundDevice::GHiOSSoundDevice(bool itunesPlaying, GHEventMgr& eventMgr, GHSoundVolumeMgr& volumeMgr)
: mOALDevice(eventMgr, &mPlatformSoundTransition, volumeMgr)
, mAVFDevice(0)
{
	if (!itunesPlaying) {
		mAVFDevice = new GHAVFSoundDevice(eventMgr, volumeMgr);
	}
}

GHiOSSoundDevice::~GHiOSSoundDevice(void)
{
	delete mAVFDevice;
}

void GHiOSSoundDevice::setListenerPosition(const GHPoint3& pos)
{
	mOALDevice.setListenerPosition(pos);
}

GHSoundEmitter* GHiOSSoundDevice::createEmitter(GHSoundHandle& sound, const GHIdentifier& category)
{
	int type = sound.getSoundHandleType();
	switch(type)
	{
		case GHSoundType_OAL:
			return mOALDevice.createEmitter(sound, category);
		case GHSoundType_AVF:
			if (mAVFDevice) {
				return mAVFDevice->createEmitter(sound, category);
			}
			return 0;
        case GHSoundType_AVPLAYER:
            if (mAVFDevice) {
                return mAVFDevice->createEmitter(sound, category);
            }
            return 0;
        default:
			assert(false && "It doesn't look like this YrgSoundHandle is a valid type for the iPhone.");
			return 0;
	}
}
