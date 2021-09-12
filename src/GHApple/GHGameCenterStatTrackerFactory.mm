// Copyright 2010 Golden Hammer Software
#include "GHGameCenterStatTrackerFactory.h"
#include "GHGameCenterStatTracker.h"
#include "GHNullStatTracker.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGameCenterFactory.h"
#include "GHGameCenterStatTrackerUIPresenter.h"
#include "GHAppShard.h"
#include "GHStatTrackerIdentifiers.h"
#include "GHGameCenterLocalPlayer.h"

class GHGameCenterLocalPlayer;

GHGameCenterStatTrackerFactory::GHGameCenterStatTrackerFactory(GHRefCountedType<GHGameCenterFactory>* gcFactory,
                                                               const GHXMLSerializer& xmlSerializer,
                                                               const GHThreadFactory& threadFactory,
                                                               GHControllerMgr& uiControllerMgr,
                                                               const GHStringIdFactory& hasher)
: mGameCenterFactory(gcFactory)
, mXMLSerializer(xmlSerializer)
, mThreadFactory(threadFactory)
, mUIControllerMgr(uiControllerMgr)
, mHasher(hasher)
{
    mGameCenterFactory->acquire();
}

GHGameCenterStatTrackerFactory::~GHGameCenterStatTrackerFactory(void)
{
    mGameCenterFactory->release();
}

GHStatTracker* GHGameCenterStatTrackerFactory::createPlatformStatTracker(GHAppShard& appShard,
    GHAchievementNotifier* notifier) const
{
    GHRefCountedType<GHGameCenterLocalPlayer>* localPlayer = mGameCenterFactory->get()->getLocalPlayer(mUIControllerMgr, appShard.mEventMgr, appShard.mProps);
    if (!localPlayer)
    {
        return new GHNullStatTracker(appShard.mProps);
    }
    else
    {
        localPlayer->get()->addActivePropIndex(GHStatTrackerIdentifiers::GP_SUPPORTSACHIEVEMENTS);
        localPlayer->get()->addActivePropIndex(GHStatTrackerIdentifiers::GP_SUPPORTSLEADERBOARDS);
    }
    
    GHGameCenterStatTracker* ret =  new GHGameCenterStatTracker(localPlayer,
                                                                mXMLSerializer,
                                                                mThreadFactory,
                                                                notifier,
                                                                mGameCenterFactory->get()->createStatTrackerPresenter(),
                                                                appShard.mControllerMgr,
                                                                mUIControllerMgr,
                                                                mHasher);
    
    return ret;
}

