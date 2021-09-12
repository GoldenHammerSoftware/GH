// Copyright 2010 Golden Hammer Software
#include "GHGameCenterMultiplayerFactory.h"
#include "GHGameCenterMultiplayerMgr.h"
#include "GHGameCenterNetworkDataMgr.h"
#include "GHGameCenterFactory.h"
#include "GHMultiplayerIdentifiers.h"
#include "GHAppShard.h"
#include "GHGameCenterLocalPlayer.h"

GHGameCenterMultiplayerFactory::GHGameCenterMultiplayerFactory(GHRefCountedType<GHGameCenterFactory>* gcFactory,
                                                               GHMessageQueue& gameThreadQueue,
                                                               GHMessageQueue& uiThreadQueue,
                                                               GHEventMgr& uiThreadEventMgr,
                                                               GHControllerMgr& uiControllerMgr)
: mGameCenterFactory(gcFactory)
, mGameThreadQueue(gameThreadQueue)
, mUIThreadQueue(uiThreadQueue)
, mUIThreadEventMgr(uiThreadEventMgr)
, mUIControllerMgr(uiControllerMgr)
{
    mGameCenterFactory->acquire();
}

GHGameCenterMultiplayerFactory::~GHGameCenterMultiplayerFactory(void)
{
    mGameCenterFactory->release();
}

GHMultiplayerMgr* GHGameCenterMultiplayerFactory::createMultiplayerMgr(GHAppShard& appShard)
{
    GHRefCountedType<GHGameCenterLocalPlayer>* localPlayer =
    mGameCenterFactory->get()->getLocalPlayer(mUIControllerMgr, appShard.mEventMgr,
        appShard.mProps);
    if (localPlayer)
    {
        localPlayer->get()->addActivePropIndex(GHMultiplayerIdentifiers::GP_MULTIPLAYERACTIVE);
    }
    else {
        appShard.mProps.setProperty(GHMultiplayerIdentifiers::GP_MULTIPLAYERACTIVE, false);
    }
    GHGameCenterNetworkDataMgr* networkDataMgr = new GHGameCenterNetworkDataMgr(localPlayer, 
        mGameCenterFactory->get()->createMultiplayerPresenter(), mGameThreadQueue);
    return new GHGameCenterMultiplayerMgr(mUIThreadQueue,
                                          mUIThreadEventMgr,
                                          appShard.mEventMgr,
                                          networkDataMgr); 
}
