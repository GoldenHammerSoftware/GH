// Copyright 2010 Golden Hammer Software
#pragma once

//Most of the GameCenter code works on both OSX and iOS. The exception
// is the displaying of the modal dialogs, so this is an interface for
// separating the leaderboard and achievement displays by platform.
class GHGameCenterStatTrackerUIPresenter
{
public:
    virtual ~GHGameCenterStatTrackerUIPresenter(void) { }
    virtual void launchLeaderboard(void) = 0;
    virtual void launchAchievements(void) = 0;
};
