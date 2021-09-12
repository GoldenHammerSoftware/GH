// Copyright Golden Hammer Software
#include "GHSoundEmitter.h"
#include "GHSoundVolumeMgr.h"

GHSoundEmitter::GHSoundEmitter(const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr)
: mCategory(category)
, mVolumeMgr(volumeMgr)
, mUnmodifiedGain(1)
{
    mVolumeMgr.registerEmitter(*this);
}

GHSoundEmitter::~GHSoundEmitter(void)
{
    mVolumeMgr.unregisterEmitter(*this);
}

void GHSoundEmitter::setGain(float gain)
{
    mUnmodifiedGain = gain;
    updateGain();
}

void GHSoundEmitter::updateGain(void)
{
    float realGain = mVolumeMgr.modifyGain(mUnmodifiedGain, mCategory);
    applyGain(realGain);
}
