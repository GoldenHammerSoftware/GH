// Copyright 2010 Golden Hammer Software
#include "GHiOSGameCenterStatTrackerUIPresenter.h"

GHiOSGameCenterStatTrackerUIPresenter::GHiOSGameCenterStatTrackerUIPresenter(UIViewController* viewController)
{
    mViewController = [[GHGameCenterUIViewController alloc] init];
    mViewController->mTopViewController = viewController;
}

GHiOSGameCenterStatTrackerUIPresenter::~GHiOSGameCenterStatTrackerUIPresenter(void)
{
}

void GHiOSGameCenterStatTrackerUIPresenter::launchLeaderboard(void)
{
    [mViewController launchLeaderboard];
}

void GHiOSGameCenterStatTrackerUIPresenter::launchAchievements(void)
{
    [mViewController launchAchievements];
}
