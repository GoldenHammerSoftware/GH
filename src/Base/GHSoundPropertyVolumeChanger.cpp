// Copyright Golden Hammer Software
#include "GHSoundPropertyVolumeChanger.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHSoundVolumeMgr.h"

GHSoundPropertyVolumeChanger::GHSoundPropertyVolumeChanger(const GHPropertyContainer& props, const GHIdentifier& prop,
                                                           const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr)
: mProps(props)
, mProp(prop)
, mCategory(category)
, mVolumeMgr(volumeMgr)
, mLastVolume(-1)
{
}

void GHSoundPropertyVolumeChanger::update(void)
{
    float val = (float)mProps.getProperty(mProp);
	if (mLastVolume != val)
	{
		mVolumeMgr.setVolume(val, mCategory);
		mLastVolume = val;
	}
}
