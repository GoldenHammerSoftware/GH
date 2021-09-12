// Copyright Golden Hammer Software
#pragma once

#include "GHStatTrackerFactory.h"
#include "GHAchievementsData.h"

class GHXMLSerializer;

class GHStatTrackerFactoryLocal : public GHStatTrackerFactory
{
public:
	GHStatTrackerFactoryLocal(const GHXMLSerializer& xmlSerializer);

protected:
    virtual GHStatTracker* createPlatformStatTracker(GHAppShard& appShard,
                                                     GHAchievementNotifier* notifier) const;
    
private:
    // mutable because we want to pass a non-const achievements data
    //  to the tracker we create.
    mutable GHAchievementsData mAchievementsData;
	const GHXMLSerializer& mXMLSerializer;
};
