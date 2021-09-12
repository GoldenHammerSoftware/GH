// Copyright 2010 Golden Hammer Software
#pragma once

#include <queue>
#include <scoreloop/scoreloopcore.h>
#include "GHString.h"
#include "GHBBDefines.h"

class GHMutex;
class GHBBScoreloopUser;
class GHAchievementsData;
class GHPropertyContainer;

//Handles dealing with the Scoreloop API for updating achieved statuses of achievements
// and retrieving them for display.
//Also manages passing information between threads as necessary.
class GHBBScoreloopAchievements
{
public:
	GHBBScoreloopAchievements(SC_Client_h& client,
							  GHBBScoreloopUser& user,
							  GHMutex* mutex,
							  GHAchievementsData& achievementsData,
							  GHPropertyContainer& props);
	~GHBBScoreloopAchievements(void);

	void updateAchievement(const char* achievement, float percentageCompleted);
	void fetchAchievements(void);

	void gameFrameUpdate(void);
	void osFrameUpdate(void);
private:
	struct Achievement
	{
		GHString mAchievementID;
		float mPercentageComplete;
	};

	void setAchievementFetchInProgress(bool val) { mAchievementFetchInProgress = val; }

private:
	//callback methods to be called by Scoreloop
	static void onAchievementSubmitted(void* cookie, SC_Error_t result);
	static void onAchievementsLoaded(void* cookie, SC_Error_t result);

	void loadAchievements(void);
	bool synchronize(void);

private:
	GHBBScoreloopUser& mUser;
	GHPropertyContainer& mProps;
	GHAchievementsData& mAchievementsData;

	//shared client, passed in
	SC_Client_h mClient;

	//handles for submitting and viewing achievements, respectively. we own these
#ifdef GHBBPLAYBOOK
	SC_AchievementsController_h mSubmissionController;
	SC_AchievementsController_h mRetrievalController;
#else
	SC_LocalAchievementsController_h mSubmissionController;
	SC_LocalAchievementsController_h mRetrievalController;
#endif

	GHMutex* mMutex;
	std::queue<Achievement> mAchievements;
	bool mAchievementSubmissionInProgress;
	bool mAchievementFetchRequested;
	bool mAchievementFetchInProgress;
	bool mDisplayAchievements;
	bool mClearWaitingFlag;
};
