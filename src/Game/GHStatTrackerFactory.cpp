// Copyright Golden Hammer Software
#include "GHStatTrackerFactory.h"
#include "GHAppShard.h"
#include "GHStatTrackerMessageHandler.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "GHStatTracker.h"

GHStatTracker* GHStatTrackerFactory::createStatTracker(GHAppShard& appShard,
                                                       GHAchievementNotifier* notifier) const
{
    GHStatTracker* ret = createPlatformStatTracker(appShard, notifier);

    GHStatTrackerMessageHandler* statTrackerMessageHandler = new GHStatTrackerMessageHandler(appShard.mEventMgr, *ret);
    appShard.mOwnedItems->push_back(new GHTypedDeletionHandle<GHMessageHandler>(statTrackerMessageHandler));

    
    appShard.mOwnedItems->push_back(new GHTypedDeletionHandle<GHStatTracker>(ret));

    return ret;
}

