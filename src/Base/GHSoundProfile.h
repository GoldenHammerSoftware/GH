#pragma once
#include "GHString/GHIdentifier.h"
#include <map>

class GHSoundVolumeMgr;

// a description of relative sound volumes between categories.
// changes to volume values will not take effect until GHSoundVolumeMgr::apply() is called.
class GHSoundProfile
{
public:
	void setVolume(const GHIdentifier& category, float volume);
	float getVolume(const GHIdentifier& category) const;

private:
	std::map<GHIdentifier, float> mCategoryVolumes;
};
