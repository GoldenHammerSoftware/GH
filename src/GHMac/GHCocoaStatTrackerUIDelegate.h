// Copyright 2010 Golden Hammer Software
#pragma once

#import <GameKit/GameKit.h>

//A class to handle the callback that signals that the
// leaderboard or achievement dialog needs to be closed.
// Also handles launching these dialogs.
@interface GHCocoaStatTrackerUIDelegate : NSObject<GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate, GKGameCenterControllerDelegate>
{
@public
    NSWindow* mWindow;
}

- (void) initVars : (NSWindow*)window;

- (void) launchLeaderboard;
- (void) launchAchievements;

- (void) launchModalViewController : (NSViewController<GKViewController>*) viewController;
- (void) modalViewControllerDidFinish;
- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController;

- (NSViewController<GKViewController>*) getLeaderboardController;
- (NSViewController<GKViewController>*) getAchievementController;
- (GKGameCenterViewController*) getGCViewController;

@end
