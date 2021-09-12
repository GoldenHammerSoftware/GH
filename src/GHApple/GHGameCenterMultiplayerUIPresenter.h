// Copyright 2010 Golden Hammer Software
#pragma once

#import <GameKit/GameKit.h>

class GHGameCenterNetworkDataMgr;

//Most of the GameCenter code works on both OSX and iOS. The exception
// is the displaying of the modal dialogs, so this is an interface for
// separating the matchmaking display by platform.
class GHGameCenterMultiplayerUIPresenter
{
public:
    virtual ~GHGameCenterMultiplayerUIPresenter(void) { }
    virtual void present(GKTurnBasedMatchmakerViewController* matchmakerController) = 0;
    virtual void dismiss(void) = 0;
    virtual void setNetworkDataMgr(GHGameCenterNetworkDataMgr* networkDataMgr) = 0;
};
