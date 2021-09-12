// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMessageHandler.h"

class GHEventMgr;
class GHBBScoreloopAchievements;
class GHBBScoreloopHighScores;
class GHPropertyContainer;

//Handles game messages to specifically display achievements,
// friends-only high scores, or global high scores.
//We are using this for Scoreloop instead of the normal mechanism
// (GHStatTracker::displayLeaderboards and GHStatTracker::displayAchievements)
// because of the requirement to have extra tabs in the leaderboard GUI.
class GHBBScoreloopDisplayMessageHandler : public GHMessageHandler
{
public:
	GHBBScoreloopDisplayMessageHandler(GHEventMgr& eventMgr,
									   GHBBScoreloopAchievements& achievements,
							 	 	   GHBBScoreloopHighScores& highScores,
							 	 	   GHPropertyContainer& props);
	~GHBBScoreloopDisplayMessageHandler(void);

	virtual void handleMessage(const GHMessage& message);

private:
	GHEventMgr& mEventMgr;
	GHBBScoreloopAchievements& mAchievements;
	GHBBScoreloopHighScores& mHighScores;
	GHPropertyContainer& mProps;
};
