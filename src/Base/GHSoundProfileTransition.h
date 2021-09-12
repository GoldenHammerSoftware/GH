#pragma once
#include "GHUtils/GHTransition.h"
#include "GHSoundProfile.h"

// applies/removes an environment sound profile when activated/deactivated
class GHSoundProfileTransition : public GHTransition
{
public:
	GHSoundProfileTransition(GHSoundVolumeMgr& volumeMgr, const GHSoundProfile& profile);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	GHSoundVolumeMgr& mVolumeMgr;
	GHSoundProfile mSoundProfile;
};
