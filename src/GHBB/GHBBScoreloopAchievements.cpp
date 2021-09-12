// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopAchievements.h"
#include "GHMutex.h"
#include "GHDebugMessage.h"
#include "GHBBScoreloopUser.h"
#include <cassert>
#include <cctype>
#include "GHAchievementsData.h"
#include "GHBBIdentifiers.h"
#include "GHPropertyContainer.h"
#include "GHBBScoreloopAchievementsLoader.h"

GHBBScoreloopAchievements::GHBBScoreloopAchievements(SC_Client_h& client, GHBBScoreloopUser& user, GHMutex* mutex,
		GHAchievementsData& achievementsData, GHPropertyContainer& props)
: mUser(user)
, mProps(props)
, mAchievementsData(achievementsData)
, mClient(client)
, mMutex(mutex)
, mAchievementSubmissionInProgress(false)
, mAchievementFetchRequested(false)
, mAchievementFetchInProgress(false)
, mDisplayAchievements(false)
, mClearWaitingFlag(false)
{
#ifdef GHBBPLAYBOOK
	SC_Error_t errCode;
	errCode = SC_Client_CreateAchievementsController(mClient, &mSubmissionController, onAchievementSubmitted, this);
	if (errCode != SC_OK) {
		GHDebugMessage::outputString("Failed to create scoreloop achievement controller for submitting with error %d", errCode);
	}

	errCode = SC_Client_CreateAchievementsController(mClient, &mRetrievalController, onAchievementsLoaded, this);
	if (errCode != SC_OK) {
		GHDebugMessage::outputString("Failed to create scoreloop achievement controller for retrieval with error %d", errCode);
	}
#else
	SC_Error_t errCode;
	errCode = SC_Client_CreateLocalAchievementsController(mClient, &mSubmissionController, onAchievementSubmitted, this);
	if (errCode != SC_OK) {
		GHDebugMessage::outputString("Failed to create scoreloop achievement controller for submitting with error %d", errCode);
	}

	errCode = SC_Client_CreateLocalAchievementsController(mClient, &mRetrievalController, onAchievementsLoaded, this);
	if (errCode != SC_OK) {
		GHDebugMessage::outputString("Failed to create scoreloop achievement controller for retrieval with error %d", errCode);
	}
#endif

	synchronize();

	GHBBScoreloopAchievementsLoader loader;
	loader.loadAchievementsFromFile(mAchievementsData);
	mDisplayAchievements = true;
}

GHBBScoreloopAchievements::~GHBBScoreloopAchievements(void)
{
#ifdef GHBBPLAYBOOK
	SC_AchievementsController_Release(mRetrievalController);
	SC_AchievementsController_Release(mSubmissionController);
#else
	SC_LocalAchievementsController_Release(mRetrievalController);
	SC_LocalAchievementsController_Release(mSubmissionController);
#endif
}

void GHBBScoreloopAchievements::updateAchievement(const char* achievement, float percentageCompleted)
{
	GHScopedMutex scopedMutex(mMutex);
	Achievement newAchievement;
	newAchievement.mAchievementID.setConstChars(achievement, GHString::CHT_COPY);
	char* str = newAchievement.mAchievementID.getNonConstChars();
	for (; *str; ++str) {
		*str = std::tolower(*str);
	}
	newAchievement.mPercentageComplete = percentageCompleted;
	mAchievements.push(newAchievement);
}

void GHBBScoreloopAchievements::fetchAchievements(void)
{
	if (!mAchievementFetchInProgress) {
		mProps.setProperty(GHBBIdentifiers::SCORELOOP_WAITING, true);
		mAchievementFetchRequested = true;
	}
}

void GHBBScoreloopAchievements::gameFrameUpdate(void)
{
	GHScopedMutex scopedMutex(mMutex);
	if (mDisplayAchievements)
	{
		mAchievementsData.notifyOfChange();
		mDisplayAchievements = false;
	}

	if (mClearWaitingFlag) {
		mProps.setProperty(GHBBIdentifiers::SCORELOOP_WAITING, false);
		mClearWaitingFlag = false;
	}
}

void GHBBScoreloopAchievements::osFrameUpdate(void)
{
	GHScopedMutex scopedMutex(mMutex);
	if (!mAchievementSubmissionInProgress
		&& mAchievements.size())
	{
		Achievement& achievement = mAchievements.front();

		SC_Bool_t wasAchieved;
#ifdef GHBBPLAYBOOK
		SC_Error_t errCode = SC_AchievementsController_SetValueForAwardIdentifier(mSubmissionController,
																				  achievement.mAchievementID.getChars(),
																				  static_cast<int>(achievement.mPercentageComplete),
																				  &wasAchieved);
#else
		SC_Error_t errCode = SC_LocalAchievementsController_SetValueForAwardIdentifier(mSubmissionController,
																					   achievement.mAchievementID.getChars(),
																					   static_cast<int>(achievement.mPercentageComplete),
																					   &wasAchieved);
#endif
		if (errCode == SC_OK)
		{
			if (synchronize())
			{
				mAchievementSubmissionInProgress = true;
			}
			else
			{
				mAchievements.pop();
			}
		}
		else
		{
			GHDebugMessage::outputString("Error submitting scoreloop achievement %s with %%complete %f. Error code %d", achievement.mAchievementID.getChars(), achievement.mPercentageComplete, errCode);
			mAchievements.pop();
		}
	}

	if (mAchievementFetchRequested)
	{
#ifdef GHBBPLAYBOOK
		SC_Error_t errCode = SC_AchievementsController_LoadAchievements(mRetrievalController);

		if (errCode == SC_OK) {
			mAchievementFetchInProgress = true;
		}
		else {
			GHDebugMessage::outputString("Error loading achievements. Error code: %d", errCode);
			mClearWaitingFlag = true;
		}
#else
		loadAchievements();
#endif
		mAchievementFetchRequested = false;
	}
}

void GHBBScoreloopAchievements::onAchievementsLoaded(void* cookie, SC_Error_t result)
{
	GHBBScoreloopAchievements* thiss = reinterpret_cast<GHBBScoreloopAchievements*>(cookie);
	thiss->loadAchievements();
	thiss->setAchievementFetchInProgress(false);
}

void GHBBScoreloopAchievements::loadAchievements(void)
{
	SC_AchievementList_h achievementList;
#ifdef GHBBPLAYBOOK
	achievementList = SC_AchievementsController_GetAchievements(mRetrievalController);
#else
	achievementList = SC_LocalAchievementsController_GetAchievements(mRetrievalController);
#endif
	if (achievementList == 0) {
		GHDebugMessage::outputString("Failed to get achievements.");
	}

	bool didChange = false;
#ifdef GHBBPLAYBOOK
	unsigned int numAchievements = SC_AchievementList_GetAchievementsCount(achievementList);
#else
	unsigned int numAchievements = SC_AchievementList_GetCount(achievementList);
#endif
	for (unsigned int i = 0; i < numAchievements; ++i)
	{
#ifdef GHBBPLAYBOOK
		SC_Achievement_h achievement = SC_AchievementList_GetAchievement(achievementList, i);
#else
		SC_Achievement_h achievement = SC_AchievementList_GetAt(achievementList, i);
#endif
		SC_Award_h award = SC_Achievement_GetAward(achievement);
		SC_String_h identifier = SC_Award_GetIdentifier(award);
		bool isAchieved = SC_Achievement_IsAchieved(achievement);

		if (isAchieved)
		{
			didChange |= mAchievementsData.setAchieved(SC_String_GetData(identifier));
		}
	}

	mClearWaitingFlag = true;

	if (didChange) {
		mDisplayAchievements = true;
	}
}

void GHBBScoreloopAchievements::onAchievementSubmitted(void* cookie, SC_Error_t result)
{
	GHBBScoreloopAchievements* thiss = reinterpret_cast<GHBBScoreloopAchievements*>(cookie);
	GHScopedMutex scopedMutex(thiss->mMutex);
	thiss->mAchievements.pop();
	thiss->mAchievementSubmissionInProgress = false;
}

bool GHBBScoreloopAchievements::synchronize(void)
{
#ifdef GHBBPLAYBOOK
	if (SC_AchievementsController_ShouldSynchronizeAchievements(mSubmissionController) == SC_TRUE) {
		SC_Error_t errCode = SC_AchievementsController_SynchronizeAchievements(mSubmissionController);
#else
	if (SC_LocalAchievementsController_ShouldSynchronize(mSubmissionController) == SC_TRUE) {
		SC_Error_t errCode = SC_LocalAchievementsController_Synchronize(mSubmissionController);
#endif
		if (errCode != SC_OK) {
			GHDebugMessage::outputString("Failed to synchronize scoreloop achievements with error %d", errCode);
		}
		return errCode == SC_OK;
	}
	return false;
}

