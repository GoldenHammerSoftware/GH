// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHStatTrackerFactory.h"
#include "GHPlatform/GHRefCounted.h"

class GHMessageHandler;
class GHThreadFactory;
class GHXMLSerializer;
class GHGameCenterFactory;
class GHControllerMgr;
class GHStringIdFactory;

class GHGameCenterStatTrackerFactory  : public GHStatTrackerFactory
{
public:
    GHGameCenterStatTrackerFactory(GHRefCountedType<GHGameCenterFactory>* gcFactory,
                                   const GHXMLSerializer& xmlSerializer,
                                   const GHThreadFactory& threadFactory,
                                   GHControllerMgr& uiControllerMgr,
                                   const GHStringIdFactory& hasher);
    
    ~GHGameCenterStatTrackerFactory(void);
    
protected:
    virtual GHStatTracker* createPlatformStatTracker(GHAppShard& appShard,
                                                     GHAchievementNotifier* notifier) const;
    
private:
    GHRefCountedType<GHGameCenterFactory>* mGameCenterFactory;
    const GHXMLSerializer& mXMLSerializer;
    const GHThreadFactory& mThreadFactory;
    GHControllerMgr& mUIControllerMgr;
    const GHStringIdFactory& mHasher;
};
