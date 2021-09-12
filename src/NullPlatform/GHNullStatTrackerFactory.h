// Copyright Golden Hammer Software
#pragma once

#include "GHStatTrackerFactory.h"
#include "GHNullStatTracker.h"
#include "GHAppShard.h"

class GHNullStatTrackerFactory : public GHStatTrackerFactory
{
protected:
    virtual GHStatTracker* createPlatformStatTracker(GHAppShard& appShard,
                                                     GHAchievementNotifier* notifier) const
    {
        return new GHNullStatTracker(appShard.mProps);
    }
};
