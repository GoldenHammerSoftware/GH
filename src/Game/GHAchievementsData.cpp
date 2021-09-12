// Copyright Golden Hammer Software
#include "GHAchievementsData.h"
#include <cstring>
#include <algorithm>

void GHAchievementsData::clearData()
{
	mAchievements.clear();
}

void GHAchievementsData::addAchievement(const Achievement& achievement)
{
	mAchievements.push_back(achievement);
}

bool GHAchievementsData::setAchieved(const char* id)
{
	size_t numAchievements = mAchievements.size();
	for (size_t i = 0; i < numAchievements; ++i)
	{
		Achievement& achievement = mAchievements[i];
		if (!std::strcmp(id, achievement.mID.getChars()))
		{
			if (!achievement.mIsAchieved)
			{
				achievement.mIsAchieved = true;
				return true;
			}
		}
	}
	return false;
}

void GHAchievementsData::registerCallback(Callback* callback)
{
	mCallbacks.push_back(callback);
}

void GHAchievementsData::unregisterCallback(Callback* callback)
{
	std::vector<Callback*>::iterator iter = std::find(mCallbacks.begin(), mCallbacks.end(), callback);
	if (iter != mCallbacks.end())
	{
		mCallbacks.erase(iter);
	}
}

void GHAchievementsData::notifyOfChange()
{
	size_t numCallbacks = mCallbacks.size();
	for (size_t i = 0; i < numCallbacks; ++i) {
		Callback* callback = mCallbacks[i];
		callback->onScoresChanged();
	}
}
