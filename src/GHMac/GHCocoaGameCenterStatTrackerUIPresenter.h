// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGameCenterStatTrackerUIPresenter.h"
#import "GHCocoaStatTrackerUIDelegate.h"

//GHApple-to-GHCocoa wrapper for presenting achievement and leaderboard dialogs
class GHCocoaGameCenterStatTrackerUIPresenter : public GHGameCenterStatTrackerUIPresenter
{
public:
    GHCocoaGameCenterStatTrackerUIPresenter(NSWindow* window);
    ~GHCocoaGameCenterStatTrackerUIPresenter(void);
    virtual void launchLeaderboard(void);
    virtual void launchAchievements(void);
private:
    GHCocoaStatTrackerUIDelegate* mDelegate;
};
