// Copyright 2010 Golden Hammer Software
#pragma once

#include <queue>
#include <scoreloop/scoreloopcore.h>
#include "GHBBScoreloopScoresData.h"

class GHMutex;
class GHBBScoreloopUser;
class GHPropertyContainer;

//Handles dealing with the Scoreloop API for updating our high scores
// and retrieving leaderboards for display.
//Also manages passing information between threads as necessary.
class GHBBScoreloopHighScores
{
public:
	GHBBScoreloopHighScores(SC_Client_h& client,
							GHBBScoreloopUser& user,
							GHMutex* mutex,
							GHBBScoreloopScoresData& scoresData,
							GHPropertyContainer& props);
	~GHBBScoreloopHighScores(void);

	void incrementScore(int mode, int value);
	void submitScore(int mode, int value);
	void fetchLeaderboard(bool global);
	void gameFrameUpdate(void);
	void osFrameUpdate(void);

	// if isIncrement, use mScoreFetchHandle
	//  otherwise use mLeaderboardHandle
	bool findOurScoreInList(SC_Score_h& ret, bool isIncrement);

private:
	struct Score
	{
		SC_Score_h mScore;
		int mMode;
		int mValue;
	};

private:
	//callback methods to be called by Scoreloop
	static void onScoreSubmitted(void* cookie, SC_Error_t result);
	static void onLeaderboardFetched(void* cookie, SC_Error_t result);
	static void onScoreFetchedForIncrement(void* cookie, SC_Error_t result);

	void handleLeaderboardsFetched(void);
	void handleOurScoreFetchedForDisplay(void);
	void copyToGHStruct(GHBBScoreloopScoresData::Score& scoreData, SC_Score_h score) const;
	void setBuddyhood(SC_ScoresController_h& scoresController, bool buddiesOnly);

private:
	GHBBScoreloopUser& mUser;
	GHBBScoreloopScoresData& mScoresData;
	GHPropertyContainer& mProps;

	//shared client, passed in
	SC_Client_h mClient;

	GHMutex* mMutex;

	//handle for submitting scores, we own this
	SC_ScoreController_h mSubmissionHandle;
	//handle for showing score leaderboard, we own this
	SC_ScoresController_h mLeaderboardHandle;
	//handle for fetching the user's current score in the case of
	// a score increment request, we own this
	SC_ScoresController_h mScoreFetchHandle;
	//For formatting the scores. We get this from ScoreLoop, but do not have to release it
	SC_ScoreFormatter_h mScoreFormatter;

	std::queue<Score> mScores;
	std::queue<Score> mScoreIncrements;
	bool mScoreSubmissionInProgress;
	bool mLeaderboardFetchRequested;
	bool mLeaderboardFetchInProgress;
	bool mOurScoreFetchInProgress;
	bool mScoreIncrementInProgress;
	bool mUpdatedScoresData;
	bool mLeaderboardRequestIsGlobal;
	bool mClearWaitingFlag;
};
