// Copyright 2010 Golden Hammer Software
#include "GHCocoaGameCenterStatTrackerUIPresenter.h"
#import <GameKit/GameKit.h>

GHCocoaGameCenterStatTrackerUIPresenter::GHCocoaGameCenterStatTrackerUIPresenter(NSWindow* window)
{
    mDelegate = [[GHCocoaStatTrackerUIDelegate alloc] init];
    [mDelegate initVars:window];
}

GHCocoaGameCenterStatTrackerUIPresenter::~GHCocoaGameCenterStatTrackerUIPresenter(void)
{
}

void GHCocoaGameCenterStatTrackerUIPresenter::launchLeaderboard(void)
{
    [mDelegate launchLeaderboard];
}

void GHCocoaGameCenterStatTrackerUIPresenter::launchAchievements(void)
{
    [mDelegate launchAchievements];
}
