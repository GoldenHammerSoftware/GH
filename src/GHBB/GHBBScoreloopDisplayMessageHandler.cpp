// Copyright 2010 Golden Hammer Software
#include "GHBBScoreloopDisplayMessageHandler.h"
#include "GHEventMgr.h"
#include "GHBBIdentifiers.h"
#include "GHMessage.h"
#include "GHBBScoreloopHighScores.h"
#include "GHBBScoreloopAchievements.h"

GHBBScoreloopDisplayMessageHandler::GHBBScoreloopDisplayMessageHandler(GHEventMgr& eventMgr,
																	   GHBBScoreloopAchievements& achievements,
							 	 	   	   	   	   	   	   	   	   	   GHBBScoreloopHighScores& highScores,
							 	 	   	   	   	   	   	   	   	   	   GHPropertyContainer& props)
: mEventMgr(eventMgr)
, mAchievements(achievements)
, mHighScores(highScores)
, mProps(props)
{
	mEventMgr.registerListener(GHBBIdentifiers::SCORELOOPDISPLAYFRIENDS, *this);
	mEventMgr.registerListener(GHBBIdentifiers::SCORELOOPDISPLAYGLOBAL, *this);
	mEventMgr.registerListener(GHBBIdentifiers::SCORELOOPDISPLAYACHIEVEMENTS, *this);
}

GHBBScoreloopDisplayMessageHandler::~GHBBScoreloopDisplayMessageHandler(void)
{
	mEventMgr.unregisterListener(GHBBIdentifiers::SCORELOOPDISPLAYACHIEVEMENTS, *this);
	mEventMgr.unregisterListener(GHBBIdentifiers::SCORELOOPDISPLAYGLOBAL, *this);
	mEventMgr.unregisterListener(GHBBIdentifiers::SCORELOOPDISPLAYFRIENDS, *this);
}

void GHBBScoreloopDisplayMessageHandler::handleMessage(const GHMessage& message)
{
	if (message.getType() == GHBBIdentifiers::SCORELOOPDISPLAYGLOBAL)
	{
		mProps.setProperty(GHBBIdentifiers::SCORELOOP_DISPLAY_IS_GLOBAL, true);
		mHighScores.fetchLeaderboard(true);
	}
	else if (message.getType() == GHBBIdentifiers::SCORELOOPDISPLAYFRIENDS)
	{
		mProps.setProperty(GHBBIdentifiers::SCORELOOP_DISPLAY_IS_GLOBAL, false);
		mHighScores.fetchLeaderboard(false);
	}
	else if(message.getType() == GHBBIdentifiers::SCORELOOPDISPLAYACHIEVEMENTS)
	{
		mAchievements.fetchAchievements();
	}
}
