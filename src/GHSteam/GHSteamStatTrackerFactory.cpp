// Copyright 2010 Golden Hammer Software
#include "GHSteamStatTrackerFactory.h"
#include "GHSteamStatTracker.h"

GHSteamStatTrackerFactory::GHSteamStatTrackerFactory(GHSteam& steam)
	: mSteam(steam)
{

}

GHStatTracker* GHSteamStatTrackerFactory::createPlatformStatTracker(GHAppShard& appShard, GHAchievementNotifier* notifier) const
{
	return new GHSteamStatTracker(mSteam);
}
