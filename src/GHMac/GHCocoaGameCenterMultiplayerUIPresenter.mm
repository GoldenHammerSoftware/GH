// Copyright 2010 Golden Hammer Software
#include "GHCocoaGameCenterMultiplayerUIPresenter.h"

GHCocoaGameCenterMultiplayerUIPresenter::GHCocoaGameCenterMultiplayerUIPresenter(NSWindow* window)
: mWindow(window)
{
    
}

void GHCocoaGameCenterMultiplayerUIPresenter::present(GKTurnBasedMatchmakerViewController* matchmakerController)
{
    GKDialogController* sdc = [GKDialogController sharedDialogController];
    sdc.parentWindow = mWindow;
    [sdc presentViewController:matchmakerController];
}

void GHCocoaGameCenterMultiplayerUIPresenter::dismiss(void)
{
    GKDialogController* sdc = [GKDialogController sharedDialogController];
    [sdc dismiss:mWindow];
}
