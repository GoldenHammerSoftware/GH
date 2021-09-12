// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHString/GHIdentifier.h"

class GHPropertyContainer;
class GHSoundVolumeMgr;

// A controller to monitor a property and change volume when it changes.
class GHSoundPropertyVolumeChanger : public GHController
{
public:
    GHSoundPropertyVolumeChanger(const GHPropertyContainer& props, const GHIdentifier& prop,
                                 const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr);
    
	virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}

private:
    const GHPropertyContainer& mProps;
    GHIdentifier mProp;
    GHIdentifier mCategory;
    GHSoundVolumeMgr& mVolumeMgr;
	float mLastVolume;
};
