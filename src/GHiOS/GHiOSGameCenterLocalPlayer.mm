// Copyright 2010 Golden Hammer Software
#include "GHiOSGameCenterLocalPlayer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGameCenterLog.h"
#include "GHGameCenter.h"
#include "GHGameCenterNetworkDataMgr.h"

GHiOSGameCenterLocalPlayer::GHiOSGameCenterLocalPlayer(GHPropertyContainer& propertyContainer, UIViewController* viewController)
: GHGameCenterLocalPlayer(propertyContainer)
, mViewController(viewController)
{
}

void GHiOSGameCenterLocalPlayer::authenticate(void)
{
    bool gcEnabled = isEnabled();
    if (!gcEnabled) { return; }
    
    GKLocalPlayer* localPlayer = [GKLocalPlayer localPlayer];
    if (localPlayer.isAuthenticated) {
        handleAuthenticationFinished(true);
        return;
    }
    
    //I know you like authenticating, but don't authenticate while you're authenticating, dawg
    if (mAuthenticationIsInProgress) { return; }
    mAuthenticationIsInProgress = true;

    if ([localPlayer respondsToSelector:@selector(setAuthenticateHandler:)])
    {
        // ios6+ way
        [localPlayer setAuthenticateHandler:(^(UIViewController* viewcontroller, NSError *error) {
            if (viewcontroller)
            {
                // This is the please log in to gamecenter screen.
                // It makes for a pretty bad multiplayer experience to show it now.
                // Not showing it just does not use gamecenter as we are not logged in.
                // If we do show it then we should return here instead of calling handleAuthenitcationFinished.
                //[mViewController presentViewController:viewcontroller animated:YES completion:nil];
                handleAuthenticationViewController(viewcontroller);
            }
            bool authenticated = [GKLocalPlayer localPlayer].isAuthenticated;
            handleAuthenticationFinished(authenticated);
        })];
    }
    else
    {
        authenticateOldStyle();
    }
}

void GHiOSGameCenterLocalPlayer::showAuthenticationViewController(void)
{
    if (!mAuthenticateView) return;
    dispatch_async(dispatch_get_main_queue(), ^{
        [mViewController presentViewController:mAuthenticateView animated:YES completion:nil];
    });
}
