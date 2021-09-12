// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGameCenterMultiplayerUIPresenter.h"
#import <GameKit/GameKit.h>

//GHApple-to-GHCocoa wrapper for presenting the multiplayer matchmaker dialog.
class GHCocoaGameCenterMultiplayerUIPresenter : public GHGameCenterMultiplayerUIPresenter
{
public:
    GHCocoaGameCenterMultiplayerUIPresenter(NSWindow* window);
    
    virtual void present(GKTurnBasedMatchmakerViewController* matchmakerController);
    virtual void dismiss(void);
    
private:
    NSWindow* mWindow;
};
