#include "GHSoundProfile.h"

void GHSoundProfile::setVolume(const GHIdentifier& category, float volume)
{
	mCategoryVolumes[category] = volume;
}

float GHSoundProfile::getVolume(const GHIdentifier& category) const
{
	auto findIter = mCategoryVolumes.find(category);
	if (findIter == mCategoryVolumes.end()) {
		return 1.0f;
	}
	return findIter->second;
}

