// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopStatTrackerFactory.h"
#include "GHBBScoreloopStatTracker.h"
#include "GHPropertyContainer.h"
#include "GHDebugMessage.h"
#include "GHThreadFactory.h"
#include <scoreloop/scoreloopcore.h>
#include "GHAppShard.h"
#include "GHStatTrackerIdentifiers.h"
#include "GHNullStatTracker.h"

GHBBScoreloopStatTrackerFactory::GHBBScoreloopStatTrackerFactory(GHControllerMgr& uiControllerMgr,
																 GHThreadFactory& threadFactory,
																 const GHXMLSerializer& xmlSerializer,
																 GHBBEventHandlerList& handlerList,
																 GHBBScoreloopScoresData& scoresData,
																 GHAchievementsData& achievementsData,
																 const char* gameID,
																 const char* gameSecret,
																 const char* gameVersion,
																 const char* currency,
																 const char* languageCode)
: mUIControllerMgr(uiControllerMgr)
, mThreadFactory(threadFactory)
, mXMLSerializer(xmlSerializer)
, mScoresData(scoresData)
, mAchievementsData(achievementsData)
, mHandlerList(handlerList)
, mGameID(gameID, GHString::CHT_COPY)
, mGameSecret(gameSecret, GHString::CHT_COPY)
, mGameVersion(gameVersion, GHString::CHT_COPY)
, mCurrency(currency, GHString::CHT_COPY)
, mLanguageCode(languageCode, GHString::CHT_COPY)
{

}

GHStatTracker* GHBBScoreloopStatTrackerFactory::createPlatformStatTracker(GHAppShard& appShard,
																		  GHAchievementNotifier* notifier) const
{
	SC_Client_h client;
	SC_InitData_t initData;

	SC_InitData_Init(&initData);

	//these three values are optional
	initData.currentVersion = SC_INIT_CURRENT_VERSION;
	initData.minimumRequiredVersion = SC_INIT_VERSION_1_0;
	initData.runLoopType = SC_RUN_LOOP_TYPE_BPS;

	SC_Error_t errCode = SC_Client_New(&client, &initData,
										mGameID.getChars(),
										mGameSecret.getChars(),
										mGameVersion.getChars(),
										mCurrency.getChars(),
										mLanguageCode.getChars());

	if (errCode == SC_OK)
	{
		appShard.mProps.setProperty(GHStatTrackerIdentifiers::GP_SUPPORTSACHIEVEMENTS, true);
		appShard.mProps.setProperty(GHStatTrackerIdentifiers::GP_SUPPORTSLEADERBOARDS, true);
	}
	else
	{
		GHDebugMessage::outputString("Failed to create Scoreloop client with error %d. Check sc_errors.h", errCode);
		return new GHNullStatTracker(appShard.mProps);
	}

	GHMutex* mutex = mThreadFactory.createMutex();
	return new GHBBScoreloopStatTracker(mutex, mHandlerList, appShard.mEventMgr, appShard.mProps,
			initData, client, mScoresData, mAchievementsData,
			appShard.mControllerMgr, mUIControllerMgr, mXMLSerializer);
}

