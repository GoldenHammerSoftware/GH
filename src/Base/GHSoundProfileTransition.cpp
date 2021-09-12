#include "GHSoundProfileTransition.h"
#include "GHSoundVolumeMgr.h"

GHSoundProfileTransition::GHSoundProfileTransition(GHSoundVolumeMgr& volumeMgr, const GHSoundProfile& profile)
	: mVolumeMgr(volumeMgr)
	, mSoundProfile(profile)
{
}

void GHSoundProfileTransition::activate(void)
{
	mVolumeMgr.setEnvironmentProfile(mSoundProfile);
}

void GHSoundProfileTransition::deactivate(void)
{
	// todo? find a way to remember state?
	GHSoundProfile zeroProfile;
	mVolumeMgr.setEnvironmentProfile(zeroProfile);
}
