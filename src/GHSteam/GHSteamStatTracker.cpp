// Copyright 2010 Golden Hammer Software
#include "GHSteamStatTracker.h"
#include "steam_api.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHSteam.h"
#include "GHMath/GHFloat.h"

GHSteamStatTracker::GHSteamStatTracker(const GHSteam& steam)
	: m_CallbackUserStatsReceived(this, &GHSteamStatTracker::OnUserStatsReceived)
	, m_CallbackUserStatsStored(this, &GHSteamStatTracker::OnUserStatsStored)
	, m_CallbackAchievementStored(this, &GHSteamStatTracker::OnAchievementStored)
	, mSteam(steam)
{
	requestStats();
}

void GHSteamStatTracker::incrementStat(const char* category, int value)
{
	if (!SteamUserStats()) { return; }

	int stat = 0;
	SteamUserStats()->GetStat(category, &stat);
	stat += value;
	SteamUserStats()->SetStat(category, stat);
	SteamUserStats()->StoreStats();
}

void GHSteamStatTracker::setStat(const char* category, int value)
{
	if (!SteamUserStats()) { return; }

	SteamUserStats()->SetStat(category, value);
	SteamUserStats()->StoreStats();
}

int GHSteamStatTracker::getStat(const char* category)
{
	if (!SteamUserStats()) { return 0; }

	int stat = 0;
	SteamUserStats()->GetStat(category, &stat);
	return stat;
}

void GHSteamStatTracker::incrementScore(const char* category, int value)
{

}

void GHSteamStatTracker::setScore(const char* category, int value)
{

}

void GHSteamStatTracker::displayLeaderboards(void)
{

}

void GHSteamStatTracker::updateAchievement(const char* achievement, float percentageCompleted)
{
	if (!SteamUserStats()) { return; }

	if (GHFloat::isEqual(percentageCompleted, 1.0f))
	{
		SteamUserStats()->SetAchievement(achievement);
	}
}

void GHSteamStatTracker::displayAchievements(void)
{

}

const char* GHSteamStatTracker::getLocalPlayerAlias(void) const
{
	if (!SteamFriends()) { return 0; }

	return SteamFriends()->GetPersonaName();
}

uint64_t GHSteamStatTracker::getLocalPlayerIdentifier(void) const
{
	if (!SteamUser()) { return 0; }

	CSteamID steamid = SteamUser()->GetSteamID();
	uint64 numericalid = steamid.ConvertToUint64();
	return numericalid;
}

GHStatTracker::PlayerIDType GHSteamStatTracker::getPlayerIDType(void) const
{
	return PID_STEAM;
}

void GHSteamStatTracker::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
//	GHDebugMessage::outputString("in GHSteamStatTracker::OnUserStatsReceived");
	if (pCallback->m_nGameID != mSteam.getAppID()) {
		return;
	}

	if (pCallback->m_eResult != k_EResultOK) {
		mSteam.printErrorResult("error on steam user stats received", pCallback->m_eResult);
		return;
	}
}

void GHSteamStatTracker::OnUserStatsStored(UserStatsStored_t* pCallback)
{
	//GHDebugMessage::outputString("in GHSteamStatTracker::OnUserStatsStored");
}

void GHSteamStatTracker::OnAchievementStored(UserAchievementStored_t* pCallback)
{
	//GHDebugMessage::outputString("in GHSteamStatTracker::OnAchievementStored");
}

void GHSteamStatTracker::requestStats(void)
{
	if (NULL == SteamUserStats() || NULL == SteamUser()
		)//|| !SteamUser()->BLoggedOn())
	{
		return;
	}

	SteamUserStats()->RequestCurrentStats();
}
