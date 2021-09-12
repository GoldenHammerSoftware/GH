// Copyright 2010 Golden Hammer Software
#include "GHCocoaGameCenterLocalPlayer.h"
#include "GHDebugMessage.h"
#include "GHGameCenterLog.h"
#include "GHGameCenter.h"
#include "GHGameCenterNetworkDataMgr.h"

GHCocoaGameCenterLocalPlayer::GHCocoaGameCenterLocalPlayer(GHPropertyContainer& propertyContainer)
: GHGameCenterLocalPlayer(propertyContainer)
{
}

void GHCocoaGameCenterLocalPlayer::authenticate(void)
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
    
    mAuthenticationIsInProgress = true;
    if ([localPlayer respondsToSelector:@selector(setAuthenticateHandler:)])
    {
        // ios6+ way
        [localPlayer setAuthenticateHandler:(^(NSViewController* viewcontroller, NSError *error) {
            if (viewcontroller)
            {
                // possibly todo: find a way to display the view controller.
                // does not seem to be necessary on osx.
                //[[AppDelegate sharedDelegate].viewController presentViewController:viewcontroller animated:YES completion:nil];
                GHDebugMessage::outputString("Got a gamecenter failure view controller that is not shown");
            }
            else if (error)
            {
                GHDebugMessage::outputString("Failed to authenticate GameCenter");
                NSLog(@"%@", error);
                handleAuthenticationFinished(false);
            }
            else
            {
                handleAuthenticationFinished(true);
            }
        })];
    }
    else
    {
        authenticateOldStyle();
    }
}
