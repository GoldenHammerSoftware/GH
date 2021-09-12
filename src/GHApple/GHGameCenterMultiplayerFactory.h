// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMultiPlayerFactory.h"
//#include <UIKit/UIKit.h>
#include "GHPlatform/GHRefCounted.h"

class GHMessageQueue;
class GHGameCenterFactory;
class GHEventMgr;
class GHControllerMgr;

class GHGameCenterMultiplayerFactory : public GHMultiplayerFactory
{
public:
    GHGameCenterMultiplayerFactory(GHRefCountedType<GHGameCenterFactory>* gcFactory,
                                   GHMessageQueue& gameThreadQueue,
                                   GHMessageQueue& uiThreadQueue,
                                   GHEventMgr& uiThreadEventMgr,
                                   GHControllerMgr& uiControllerMgr);
    ~GHGameCenterMultiplayerFactory(void);
    
    virtual GHMultiplayerMgr* createMultiplayerMgr(GHAppShard& appShard);
    
private:
    GHRefCountedType<GHGameCenterFactory>* mGameCenterFactory;
    GHMessageQueue& mGameThreadQueue;
    GHMessageQueue& mUIThreadQueue;
    GHEventMgr& mUIThreadEventMgr;
    GHControllerMgr& mUIControllerMgr;
};
