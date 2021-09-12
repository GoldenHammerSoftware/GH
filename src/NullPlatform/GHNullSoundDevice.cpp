// Copyright Golden Hammer Software
#include "GHNullSoundDevice.h"
#include "GHNullSoundEmitter.h"

GHNullSoundDevice::GHNullSoundDevice(GHSoundVolumeMgr& volumeMgr)
: mVolumeMgr(volumeMgr)
{
}

void GHNullSoundDevice::enable3DSound(bool)
{

}

void GHNullSoundDevice::setListenerPosition(const GHPoint3& )
{
}

void GHNullSoundDevice::setListenerOrientation(const GHTransform&)
{

}

void GHNullSoundDevice::setListenerVelocity(const GHPoint3&)
{

}

GHSoundEmitter* GHNullSoundDevice::createEmitter(GHSoundHandle&, const GHIdentifier& category)
{
    return new GHNullSoundEmitter(category, mVolumeMgr);
}
