// Copyright Golden Hammer Software
#include "GHStatTrackerMessageHandler.h"
#include "GHUtils/GHEventMgr.h"
#include "GHStatTracker.h"
#include "GHStatTrackerIdentifiers.h"
#include "GHUtils/GHMessage.h"

GHStatTrackerMessageHandler::GHStatTrackerMessageHandler(GHEventMgr& eventMgr,
                                                         GHStatTracker& statTracker)
: mEventMgr(eventMgr)
, mStatTracker(statTracker)
{
    mEventMgr.registerListener(GHStatTrackerIdentifiers::MSG_SHOWACHIEVEMENTS, *this);
    mEventMgr.registerListener(GHStatTrackerIdentifiers::MSG_SHOWLEADERBOARD, *this);
}

GHStatTrackerMessageHandler::~GHStatTrackerMessageHandler(void)
{
    mEventMgr.unregisterListener(GHStatTrackerIdentifiers::MSG_SHOWLEADERBOARD, *this);
    mEventMgr.unregisterListener(GHStatTrackerIdentifiers::MSG_SHOWACHIEVEMENTS, *this);
}

void GHStatTrackerMessageHandler::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHStatTrackerIdentifiers::MSG_SHOWACHIEVEMENTS)
    {
        mStatTracker.displayAchievements();
    }
    else if (message.getType() == GHStatTrackerIdentifiers::MSG_SHOWLEADERBOARD)
    {
        mStatTracker.displayLeaderboards();
    }
}
