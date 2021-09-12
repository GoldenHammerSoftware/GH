// Copyright Golden Hammer Software
#pragma once

class GHStatTracker;
class GHAppShard;
class GHAchievementNotifier;
class GHIdentifier;

class GHStatTrackerFactory
{
public:
    virtual ~GHStatTrackerFactory(void) { }
    
    // call createPlatformStatTracker,
    //  then do any initialization shared between platforms.
    GHStatTracker* createStatTracker(GHAppShard& appShard,
                                     GHAchievementNotifier* notifier) const;
    
protected:
    // Create GHGameCenterStatTracker for example.
    virtual GHStatTracker* createPlatformStatTracker(GHAppShard& appShard,
                                                     GHAchievementNotifier* notifier) const = 0;
};
