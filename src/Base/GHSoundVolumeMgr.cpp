// Copyright Golden Hammer Software
#include "GHSoundVolumeMgr.h"
#include "GHSoundEmitter.h"
#include "GHMath/GHFloat.h"

GHSoundVolumeMgr::GHSoundVolumeMgr(void)
{
}

float GHSoundVolumeMgr::modifyGain(float rawGain, const GHIdentifier& category)
{
    float gainMult = mGlobalSoundProfile.getVolume(0);
	gainMult *= mGlobalSoundProfile.getVolume(category);
	gainMult *= mEnvironmentSoundProfile.getVolume(category);
    return (rawGain*gainMult);
}

void GHSoundVolumeMgr::setVolume(float val, const GHIdentifier& category)
{
    if (GHFloat::isEqual(val, getVolume(category))) {
        return;
    }

    mGlobalSoundProfile.setVolume(category, val);
    updateEmitterVolumes();
}

float GHSoundVolumeMgr::getVolume(const GHIdentifier& category)
{
	return mGlobalSoundProfile.getVolume(category);
}

void GHSoundVolumeMgr::registerEmitter(GHSoundEmitter& emitter)
{
    mEmitters.push_back(&emitter);
}

void GHSoundVolumeMgr::unregisterEmitter(GHSoundEmitter& emitter)
{
    std::list<GHSoundEmitter*>::iterator findIter;
    for (findIter = mEmitters.begin(); findIter != mEmitters.end(); ++findIter)
    {
        if (*findIter == &emitter) {
            mEmitters.erase(findIter);
            return;
        }
    }
}

void GHSoundVolumeMgr::updateEmitterVolumes(void) const
{
    std::list<GHSoundEmitter*>::const_iterator iter;
    for (iter = mEmitters.begin(); iter != mEmitters.end(); ++iter)
    {
        (*iter)->updateGain();
    }
}

void GHSoundVolumeMgr::setEnvironmentProfile(const GHSoundProfile& envProfile)
{
	mEnvironmentSoundProfile = envProfile;
}
