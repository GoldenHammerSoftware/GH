// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"

class GHEventMgr;
class GHStatTracker;

//Displays Leaderboard and Achivement UIs, listens for the events to trigger them.
class GHStatTrackerMessageHandler : public GHMessageHandler
{
public:
    GHStatTrackerMessageHandler(GHEventMgr& eventMgr,
                                GHStatTracker& statTracker);
    ~GHStatTrackerMessageHandler(void);
    
    virtual void handleMessage(const GHMessage& message);
    
private:
    GHEventMgr& mEventMgr;
    GHStatTracker& mStatTracker;
};
