// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopHighScores.h"
#include "GHMutex.h"
#include "GHDebugMessage.h"
#include "GHBBScoreloopUser.h"
#include <cassert>
#include "GHBBScoreloopScoresData.h"
#include "GHBBIdentifiers.h"
#include "GHPropertyContainer.h"
#include "GHBBDefines.h"

GHBBScoreloopHighScores::GHBBScoreloopHighScores(SC_Client_h& client,
												 GHBBScoreloopUser& user,
												 GHMutex* mutex,
												 GHBBScoreloopScoresData& scoresData,
												 GHPropertyContainer& props)
: mUser(user)
, mScoresData(scoresData)
, mProps(props)
, mClient(client)
, mMutex(mutex)
, mScoreSubmissionInProgress(false)
, mLeaderboardFetchRequested(false)
, mLeaderboardFetchInProgress(false)
, mOurScoreFetchInProgress(false)
, mScoreIncrementInProgress(false)
, mUpdatedScoresData(false)
, mLeaderboardRequestIsGlobal(false)
, mClearWaitingFlag(false)
{
	SC_Error_t errCode;
	errCode = SC_Client_CreateScoreController(mClient, &mSubmissionHandle, onScoreSubmitted, this);
	if (errCode != SC_OK) {
		GHDebugMessage::outputString("Failed to create Scoreloop ScoreController with error %d", errCode);
	}

	errCode = SC_Client_CreateScoresController(mClient, &mLeaderboardHandle, onLeaderboardFetched, this);
	if (errCode != SC_OK) {
		GHDebugMessage::outputString("Failed to create Scoreloop leaderboard handle with error %d", errCode);
	}

	errCode = SC_Client_CreateScoresController(mClient, &mScoreFetchHandle, onScoreFetchedForIncrement, this);
	if (errCode != SC_OK) {
		GHDebugMessage::outputString("Failed to create scoreloop score fetch handle with error %d", errCode);
	}

	mScoreFormatter = 0;
#ifdef GHBBPLAYBOOK
	errCode = SC_Client_GetScoreFormatter(mClient, &mScoreFormatter);
	if (errCode != SC_OK) {
		GHDebugMessage::outputString("Failed to create scoreloop score formatter with error %d", errCode);
	}
#else
	mScoreFormatter = SC_Client_GetScoreFormatter(mClient);
	if (mScoreFormatter == 0) {
		GHDebugMessage::outputString("Failed to create scoreloop score formatter.");
	}
#endif


	//Todo: make it so that we can fetch more than one type of leaderboard
	SC_ScoresController_SetMode(mLeaderboardHandle, 0);
	SC_ScoresController_SetMode(mScoreFetchHandle, 0);

	setBuddyhood(mScoreFetchHandle, true);
}

GHBBScoreloopHighScores::~GHBBScoreloopHighScores(void)
{
	//Todo: save unsubmitted scores to file
	while (mScores.size())
	{
		Score& score = mScores.front();
		SC_Score_Release(score.mScore);
		mScores.pop();
	}

	SC_ScoresController_Release(mScoreFetchHandle);
	SC_ScoresController_Release(mLeaderboardHandle);
	SC_ScoreController_Release(mSubmissionHandle);

	delete mMutex;
}

void GHBBScoreloopHighScores::incrementScore(int mode, int value)
{
	GHScopedMutex scopedMutex(mMutex);
	Score score;
	score.mMode = mode;
	score.mValue = value;
	mScoreIncrements.push(score);
}

void GHBBScoreloopHighScores::submitScore(int mode, int value)
{
	GHScopedMutex scopedMutex(mMutex);
	Score score;
	score.mMode = mode;
	score.mValue = value;
	mScores.push(score);
}

void GHBBScoreloopHighScores::fetchLeaderboard(bool global)
{
	GHScopedMutex scopedMutex(mMutex);
	if (!mLeaderboardFetchInProgress) {
		mLeaderboardRequestIsGlobal = global;
		mLeaderboardFetchRequested = true;
		mScoresData.clearData();
		mScoresData.notifyOfChange();
		mProps.setProperty(GHBBIdentifiers::SCORELOOP_WAITING, true);
	}
}

void GHBBScoreloopHighScores::gameFrameUpdate(void)
{
	GHScopedMutex scopedMutex(mMutex);
	if (mUpdatedScoresData) {
		mScoresData.notifyOfChange();
		mUpdatedScoresData = false;
	}
	if (mClearWaitingFlag) {
		mProps.setProperty(GHBBIdentifiers::SCORELOOP_WAITING, false);
		mClearWaitingFlag = false;
	}
}

void GHBBScoreloopHighScores::osFrameUpdate(void)
{
	GHScopedMutex scopedMutex(mMutex);
	if (!mScoreSubmissionInProgress
		&& mScores.size())
	{
		Score& score = mScores.front();

		SC_Error_t errCode = SC_Client_CreateScore(mClient, &score.mScore);
		if (errCode != SC_OK) {
			mScores.pop();
			return;
		}

		SC_Score_SetResult(score.mScore, static_cast<double>(score.mValue));
		SC_Score_SetMode(score.mScore, score.mMode);

		errCode = SC_ScoreController_SubmitScore(mSubmissionHandle, score.mScore);
		if (errCode != SC_OK)
		{
			GHDebugMessage::outputString("Failed to submit score");
		}
		mScoreSubmissionInProgress = true;
	}

	if (!mScoreIncrementInProgress
		&& mScoreIncrements.size()
		&& mUser.getUser() != 0)
	{
#ifdef GHBBPLAYBOOK
		SC_Error_t errCode = SC_ScoresController_LoadRangeForUser(mScoreFetchHandle, mUser.getUser(), 3);
#else
		SC_Error_t errCode = SC_ScoresController_LoadScoresAroundUser(mScoreFetchHandle, mUser.getUser(), 1);
#endif
		if (errCode != SC_OK) {
			GHDebugMessage::outputString("Failed to load user's score in order to increment it with error code %d", errCode);
		}

		mScoreIncrementInProgress = true;
	}

	if (mLeaderboardFetchRequested
		&& !mLeaderboardFetchInProgress)
	{
		mLeaderboardFetchInProgress = true;
		mLeaderboardFetchRequested = false;

		SC_Error_t errCode = 0;

		//if we aren't global, then we are looking at friends
		setBuddyhood(mLeaderboardHandle, !mLeaderboardRequestIsGlobal);

#ifdef GHBBPLAYBOOK
		errCode = SC_ScoresController_LoadRange(mLeaderboardHandle, 0, GHBBScoreloopScoresData::MaxDisplayableScores);
#else
		SC_Range_t range = { 0, GHBBScoreloopScoresData::MaxDisplayableScores };
		errCode = SC_ScoresController_LoadScores(mLeaderboardHandle, range);
#endif
		if (errCode != SC_OK) {
			GHDebugMessage::outputString("Failed to request Scoreloop leaderboard with error %d", errCode);
			mLeaderboardFetchInProgress = false;
			mUpdatedScoresData = true;
			mClearWaitingFlag = true;
		}
	}
}

void GHBBScoreloopHighScores::setBuddyhood(SC_ScoresController_h& scoresController, bool buddiesOnly)
{
#ifdef GHBBPLAYBOOK
		SC_Error_t errCode;
		if (buddiesOnly)
		{
			errCode = SC_ScoresController_SetBuddyhoodOnly(scoresController, mUser.getUser());
		}
		else
		{
			errCode = SC_ScoresController_SetBuddyhoodOnly(scoresController, 0);
		}

		if (errCode != SC_OK) {
			GHDebugMessage::outputString("Failed to set buddyhood-only for score search to %s with error %d", mLeaderboardRequestIsGlobal ? "false" : "true", errCode);
		}
#else
		SC_ScoresSearchList_t searchList = SC_SCORES_SEARCH_LIST_ALL;
		if (buddiesOnly)
		{
			searchList.usersSelector = SC_USERS_SELECTOR_BUDDYHOOD;
			searchList.buddyhoodUser = mUser.getUser();
		}
		SC_ScoresController_SetSearchList(scoresController, searchList);
#endif
}

void GHBBScoreloopHighScores::onScoreSubmitted(void* cookie, SC_Error_t result)
{
	GHBBScoreloopHighScores* thiss = reinterpret_cast<GHBBScoreloopHighScores*>(cookie);
	Score& score = thiss->mScores.front();

	if (result == SC_HTTP_SERVER_ERROR)
	{
		//todo: store the score locally
		GHDebugMessage::outputString("Scoreloop server error");
	}
	else if (result != SC_OK) {
		GHDebugMessage::outputString("There was an error submitting a Scoreloop score. Error code: %d", result);
	}

	SC_Score_Release(score.mScore);
	thiss->mScores.pop();

	thiss->mScoreSubmissionInProgress = false;
}


void GHBBScoreloopHighScores::onLeaderboardFetched(void* cookie, SC_Error_t result)
{
	GHBBScoreloopHighScores* thiss = reinterpret_cast<GHBBScoreloopHighScores*>(cookie);
	GHScopedMutex scopedMutex(thiss->mMutex);
	if (result != SC_OK)
	{
		GHDebugMessage::outputString("Failed to load leaderboards from Scoreloop.");
		thiss->mLeaderboardFetchInProgress = false;
		thiss->mOurScoreFetchInProgress = false;
		thiss->mClearWaitingFlag = true;
		thiss->mUpdatedScoresData = true;
		return;
	}

	if (thiss->mOurScoreFetchInProgress)
	{
		thiss->handleOurScoreFetchedForDisplay();
	}
	else
	{
		thiss->handleLeaderboardsFetched();
	}
}

void GHBBScoreloopHighScores::handleLeaderboardsFetched(void)
{
	SC_ScoreList_h scoreList = SC_ScoresController_GetScores(mLeaderboardHandle);
	if (scoreList == NULL) {
		GHDebugMessage::outputString("Failed to get scores from Scoreloop");
		mLeaderboardFetchInProgress = false;
		mClearWaitingFlag = true;
		mUpdatedScoresData = true;
		return;
	}
#ifdef GHBBPLAYBOOK
	const unsigned int numScores = SC_ScoreList_GetScoresCount(scoreList);
#else
	const unsigned int numScores = SC_ScoreList_GetCount(scoreList);
#endif
	for (unsigned int i = 0; i < numScores; ++i)
	{
#ifdef GHBBPLAYBOOK
		SC_Score_h score = SC_ScoreList_GetScore(scoreList, i);
#else
		SC_Score_h score = SC_ScoreList_GetAt(scoreList, i);
#endif
		GHBBScoreloopScoresData::Score scoreData;
		copyToGHStruct(scoreData, score);
		mScoresData.addScore(scoreData);
	}
	setBuddyhood(mLeaderboardHandle, true);
#ifdef GHBBPLAYBOOK
	SC_Error_t errCode = SC_ScoresController_LoadRangeForUser(mLeaderboardHandle, mUser.getUser(), 3);
#else
	SC_Error_t errCode = SC_ScoresController_LoadScoresAroundUser(mLeaderboardHandle, mUser.getUser(), 1);
#endif
	if (errCode != SC_OK) {
		GHDebugMessage::outputString("Failed to load user's score in order to display it with error code %d", errCode);
		mLeaderboardFetchInProgress = false;
	}
	else mOurScoreFetchInProgress = true;

	//note: we don't set mLeaderboardFetchInProgress to false yet because
	// we are waiting for the single user's score to fetch.
}

void GHBBScoreloopHighScores::copyToGHStruct(GHBBScoreloopScoresData::Score& scoreData, SC_Score_h score) const
{
	SC_User_h user = SC_Score_GetUser(score);
	SC_String_h login = user ? SC_User_GetLogin(user) : 0;

	bool scoreWasFormatted = false;

	if (mScoreFormatter != 0)
	{
		SC_String_h formattedScore;
		SC_Error_t errCode = SC_ScoreFormatter_FormatScore(mScoreFormatter, score, SC_SCORE_FORMAT_SCORES_ONLY, &formattedScore);
		if (errCode == SC_OK)
		{
			scoreData.mScore.setConstChars(SC_String_GetData(formattedScore), GHString::CHT_COPY);
			SC_String_Release(formattedScore);
			scoreWasFormatted = true;
		}
	}

	if(!scoreWasFormatted)
	{
		int result = (int)SC_Score_GetResult(score);
		char buf[8];
		snprintf(buf, 64, "%d", result);
		scoreData.mScore.setConstChars(buf, GHString::CHT_COPY);
	}

	scoreData.mUsername.setConstChars(login ? SC_String_GetData(login) : "Unknown", GHString::CHT_COPY);
	scoreData.mRank = SC_Score_GetRank(score);
}

bool GHBBScoreloopHighScores::findOurScoreInList(SC_Score_h& ret, bool isIncrement)
{
	SC_User_h myUser = mUser.getUser();

	SC_ScoreList_h scoreList;
	if (isIncrement) {
		scoreList = SC_ScoresController_GetScores(mScoreFetchHandle);
	}
	else {
		scoreList = SC_ScoresController_GetScores(mLeaderboardHandle);
	}
	if (scoreList == NULL) {
		SC_String_h login = myUser ? SC_User_GetLogin(myUser) : 0;
		GHDebugMessage::outputString("Got an empty score list for player %s from Scoreloop", SC_String_GetData(login));
		return false;
	}

#ifdef GHBBPLAYBOOK
	const unsigned int numScores = SC_ScoreList_GetScoresCount(scoreList);
#else
	const unsigned int numScores = SC_ScoreList_GetCount(scoreList);
#endif
	for (int i = 0; i < numScores; ++i)
	{
#ifdef GHBBPLAYBOOK
		ret = SC_ScoreList_GetScore(scoreList, i);
#else
		ret = SC_ScoreList_GetAt(scoreList, i);
#endif
		SC_User_h user = SC_Score_GetUser(ret);
		if (SC_User_Equals(myUser, user) == SC_TRUE)
		{
			return true;
		}
		/*
		// test hack
		else {
			SC_String_h myLogin = myUser ? SC_User_GetLogin(myUser) : 0;
			SC_String_h otherLogin = user ? SC_User_GetLogin(user) : 0;
			GHDebugMessage::outputString("Users %s and %s do not match", SC_String_GetData(myLogin), SC_String_GetData(otherLogin));
		}
		*/
	}

	GHDebugMessage::outputString("Failed to find player's score from Scoreloop");
	return false;
}

void GHBBScoreloopHighScores::handleOurScoreFetchedForDisplay(void)
{
	SC_Score_h myScore;
	bool found = findOurScoreInList(myScore, false);
	if (!found)
	{
		SC_User_h myUser = mUser.getUser();
		SC_String_h login = myUser ? SC_User_GetLogin(myUser) : 0;
		GHDebugMessage::outputString("Failed to get player %s score from Scoreloop for display", SC_String_GetData(login));
		mLeaderboardFetchInProgress = false;
		mOurScoreFetchInProgress = false;
		mClearWaitingFlag = true;
		mUpdatedScoresData = true;
		return;
	}

	GHBBScoreloopScoresData::Score ghScore;
	copyToGHStruct(ghScore, myScore);
	mScoresData.setOurScore(ghScore);

	mOurScoreFetchInProgress = false;
	mLeaderboardFetchInProgress = false;
	mUpdatedScoresData = true;
	mClearWaitingFlag = true;
}

void GHBBScoreloopHighScores::onScoreFetchedForIncrement(void* cookie, SC_Error_t result)
{
	GHBBScoreloopHighScores* thiss = reinterpret_cast<GHBBScoreloopHighScores*>(cookie);
	GHScopedMutex scopedMutex(thiss->mMutex);
	thiss->mScoreIncrementInProgress = false;

	if (result != SC_OK) {
		GHDebugMessage::outputString("Failure in fetching the score for incrementing with error code %d", result);
		return;
	}

	Score& ghScore = thiss->mScoreIncrements.front();

	SC_Score_h myScore;
	bool found = thiss->findOurScoreInList(myScore, true);
	if (!found)
	{
		GHDebugMessage::outputString("Failed to find our score for increment");
	}
	else
	{
		double value = SC_Score_GetResult(myScore);
		ghScore.mValue += static_cast<int>(value);
	}
	thiss->mScores.push(ghScore);
	thiss->mScoreIncrements.pop();
}

