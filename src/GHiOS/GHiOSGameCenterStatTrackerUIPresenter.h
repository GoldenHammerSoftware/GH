// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGameCenterStatTrackerUIPresenter.h"
#import "GHGameCenterUIViewController.h"

class GHiOSGameCenterStatTrackerUIPresenter : public GHGameCenterStatTrackerUIPresenter
{
public:
    GHiOSGameCenterStatTrackerUIPresenter(UIViewController* viewController);
    ~GHiOSGameCenterStatTrackerUIPresenter(void);
    virtual void launchLeaderboard(void);
    virtual void launchAchievements(void);
    
private:
    GHGameCenterUIViewController* mViewController;
};
