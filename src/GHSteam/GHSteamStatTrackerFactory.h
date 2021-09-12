// Copyright 2010 Golden Hammer Software
#pragma once

#include "Game/GHStatTrackerFactory.h"

class GHSteam;

class GHSteamStatTrackerFactory : public GHStatTrackerFactory
{
public:
	GHSteamStatTrackerFactory(GHSteam& steam);

protected:
	virtual GHStatTracker* createPlatformStatTracker(GHAppShard& appShard,
													 GHAchievementNotifier* notifier) const;

private:
	GHSteam& mSteam;
};
