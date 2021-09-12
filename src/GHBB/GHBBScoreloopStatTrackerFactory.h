// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHStatTrackerFactory.h"
#include "GHString.h"
#include "GHBBEventHandler.h"
#include "GHThreadFactory.h"

class GHBBScoreloopScoresData;
class GHAchievementsData;
class GHControllerMgr;
class GHXMLSerializer;

//Creates the GHScoreloop implementation of the GHStatTracker.
//Also handles Scoreloop initialization.
class GHBBScoreloopStatTrackerFactory : public GHStatTrackerFactory
{
public:
	GHBBScoreloopStatTrackerFactory(GHControllerMgr& uiControllerMgr,
									GHThreadFactory& threadFactory,
									const GHXMLSerializer& xmlSerializer,
									GHBBEventHandlerList& handlerList,
									GHBBScoreloopScoresData& scoresData,
									GHAchievementsData& achievementsData,
									const char* gameID,
									const char* gameSecret,
									const char* gameVersion,
									const char* currency,
									const char* languageCode);

protected:
    virtual GHStatTracker* createPlatformStatTracker(GHAppShard& appShard,
													 GHAchievementNotifier* notifier) const;

private:
    GHControllerMgr& mUIControllerMgr;
    GHThreadFactory& mThreadFactory;
    const GHXMLSerializer& mXMLSerializer;
    GHBBScoreloopScoresData& mScoresData;
    GHAchievementsData& mAchievementsData;
    GHBBEventHandlerList& mHandlerList;
    GHString mGameID;
    GHString mGameSecret;
    GHString mGameVersion;
    GHString mCurrency;
    GHString mLanguageCode;
};

