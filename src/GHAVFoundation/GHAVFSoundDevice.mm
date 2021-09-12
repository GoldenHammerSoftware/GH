// Copyright 2010 Golden Hammer Software
#include "GHAVFSoundDevice.h"
#include "GHAVFSoundEmitter.h"
#include "GHAVPSoundEmitter.h"
#include "GHAVFSoundHandle.h"
#include "GHUtils/GHEventMgr.h"

GHAVFSoundDevice::GHAVFSoundDevice(GHEventMgr& eventMgr, GHSoundVolumeMgr& volumeMgr)
: mEmitterMgr(eventMgr)
, mVolumeMgr(volumeMgr)
{
	GHAVFSoundHandle::setMessageHandler(&eventMgr);
}

GHAVFSoundDevice::~GHAVFSoundDevice(void)
{
}

void GHAVFSoundDevice::setListenerPosition(const GHPoint3& pos)
{
}

GHSoundEmitter* GHAVFSoundDevice::createEmitter(GHSoundHandle& sound, const GHIdentifier& category)
{
    if (sound.getSoundHandleType() == GHSoundType_AVPLAYER)
    {
        return new GHAVPSoundEmitter((GHAVPSoundHandle&)sound, mEmitterMgr, category, mVolumeMgr);
    }
    return new GHAVFSoundEmitter((GHAVFSoundHandle&)sound, mEmitterMgr, category, mVolumeMgr);
}
