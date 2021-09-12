// Copyright 2010 Golden Hammer Software
#import "GHCocoaStatTrackerUIDelegate.h"

@implementation GHCocoaStatTrackerUIDelegate

- (void) initVars : (NSWindow*)window
{
    mWindow = window;
}

- (void) launchLeaderboard
{
    NSViewController<GKViewController>* vc = [self getLeaderboardController];
    [self launchModalViewController:vc];
}

- (void) launchAchievements
{
    NSViewController<GKViewController>* vc = [self getAchievementController];
    [self launchModalViewController:vc];
}

- (void) leaderboardViewControllerDidFinish : (GKLeaderboardViewController*) leaderBoard
{
    [self modalViewControllerDidFinish];
}

- (void) achievementViewControllerDidFinish : (GKAchievementViewController*) achievements
{
    [self modalViewControllerDidFinish];
}

- (void) launchModalViewController:(NSViewController<GKViewController> *)viewController
{
    if (viewController != nil)
    {
        GKDialogController* sdc = [GKDialogController sharedDialogController];
        sdc.parentWindow = mWindow;
        [viewController becomeFirstResponder];
        [sdc presentViewController:viewController];
    }
}

- (void) modalViewControllerDidFinish
{
    GKDialogController* sdc = [GKDialogController sharedDialogController];
    [sdc dismiss:self];
}

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController
{
    [self modalViewControllerDidFinish];
}

- (NSViewController<GKViewController>*) getLeaderboardController
{
    GKGameCenterViewController* gcController = [self getGCViewController];
    if (gcController != 0) {
        gcController.viewState = GKGameCenterViewControllerStateLeaderboards;
        return gcController;
    }
    GKLeaderboardViewController* leaderboardController = [[GKLeaderboardViewController alloc] init];
    [leaderboardController setLeaderboardDelegate:self];
    return leaderboardController;
}

- (NSViewController<GKViewController>*) getAchievementController
{
    GKGameCenterViewController* gcController = [self getGCViewController];
    if (gcController != 0) {
        gcController.viewState = GKGameCenterViewControllerStateAchievements;
        return gcController;
    }
    GKAchievementViewController* achievementController = [[GKAchievementViewController alloc] init];
    [achievementController setAchievementDelegate:self];
    return achievementController;
}

- (GKGameCenterViewController*) getGCViewController
{
    BOOL newViewAvailable = (NSClassFromString(@"GKGameCenterViewController")) != nil;
    if (newViewAvailable == FALSE) {
        return 0;
    }
    GKGameCenterViewController* gcViewController = [[GKGameCenterViewController alloc] init];
    gcViewController.gameCenterDelegate = self;
    return gcViewController;
}

@end
