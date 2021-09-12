// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGameCenterMultiplayerUIPresenter.h"
#import <GameKit/GameKit.h>

class GHiOSGameCenterMultiplayerUIPresenter : public GHGameCenterMultiplayerUIPresenter
{
public:
    GHiOSGameCenterMultiplayerUIPresenter(UIViewController* viewController);
    
    virtual void present(GKTurnBasedMatchmakerViewController* matchmakerController);
    virtual void dismiss(void);
    virtual void setNetworkDataMgr(GHGameCenterNetworkDataMgr* networkDataMgr) { mNetworkDataMgr = networkDataMgr; }
    
private:
    UIViewController* mViewController;
    GHGameCenterNetworkDataMgr* mNetworkDataMgr{ nullptr };
};
