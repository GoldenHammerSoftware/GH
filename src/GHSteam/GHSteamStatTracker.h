// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHStatTracker.h"
#include <steam_api.h>
#include "GHString/GHString.h"
#include <vector>

class GHSteam;

class GHSteamStatTracker : public GHStatTracker
{
public:

	GHSteamStatTracker(const GHSteam& steam);

	virtual void incrementStat(const char* category, int value);
	virtual void setStat(const char* category, int value);
	virtual int getStat(const char* category);
	virtual void incrementScore(const char* category, int value);
	virtual void setScore(const char* category, int value);
	virtual void displayLeaderboards(void);

	virtual void updateAchievement(const char* achievement, float percentageCompleted);
	virtual void displayAchievements(void);

	virtual const char* getLocalPlayerAlias(void) const;
	virtual uint64_t getLocalPlayerIdentifier(void) const;
	virtual PlayerIDType getPlayerIDType(void) const;


	STEAM_CALLBACK(GHSteamStatTracker, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(GHSteamStatTracker, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(GHSteamStatTracker, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);

private:
	void requestStats(void);

private:
	const GHSteam& mSteam;
};
