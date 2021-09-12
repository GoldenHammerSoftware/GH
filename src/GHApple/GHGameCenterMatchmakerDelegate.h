// Copyright 2010 Golden Hammer Software
#pragma once

#import <GameKit/GameKit.h>

class GHGameCenterNetworkDataMgr;
class GHGameCenterMultiplayerUIPresenter;

@interface GHGameCenterMatchmakerDelegate : NSObject <GKTurnBasedMatchmakerViewControllerDelegate,
                                                      GKLocalPlayerListener>
{
@public
    GHGameCenterMultiplayerUIPresenter* mUIPresenter;
    GHGameCenterNetworkDataMgr* mMultiplayerMgr;
    GKTurnBasedMatch* mCurrentMatch;
}

- (void) setPresenter:(GHGameCenterMultiplayerUIPresenter*)uiPresenter andMultiplayerMgr:(GHGameCenterNetworkDataMgr*)multiplayerMgr;
- (void) clearCurrentMatch;
- (void) setCurrentMatch:(GKTurnBasedMatch*)match;
- (bool) itIsOurTurn;
- (void) reloadCurrentMatch:(GKTurnBasedMatch*)match;

@end
