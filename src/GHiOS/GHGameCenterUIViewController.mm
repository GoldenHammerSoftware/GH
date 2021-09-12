// Copyright 2010 Golden Hammer Software
#import "GHGameCenterUIViewController.h"

@implementation GHGameCenterUIViewController

- (void) launchLeaderboard
{
    // ios6 changed the interface.
    BOOL newViewAvailable = (NSClassFromString(@"GKGameCenterViewController")) != nil;
    
    if (newViewAvailable)
    {
        GKGameCenterViewController* gcvc = [[GKGameCenterViewController alloc] init];
        if (gcvc != nil)
        {
            gcvc.viewState = GKGameCenterViewControllerStateLeaderboards;
            gcvc.gameCenterDelegate = self;
            [self launchModalViewController:gcvc];
        }
    
    }
    else
    {
        GKLeaderboardViewController *leaderBoard = [[GKLeaderboardViewController alloc] init];
        if (leaderBoard != nil)
        {
            leaderBoard.leaderboardDelegate = self;
            [self launchModalViewController:leaderBoard];
        }
    }
}

- (void) launchAchievements
{
    // ios6 changed the interface.
    BOOL newViewAvailable = (NSClassFromString(@"GKGameCenterViewController")) != nil;
    
    if (newViewAvailable)
    {
        GKGameCenterViewController* gcvc = [[GKGameCenterViewController alloc] init];
        if (gcvc != nil)
        {
            gcvc.viewState = GKGameCenterViewControllerStateAchievements;
            gcvc.gameCenterDelegate = self;
            [self launchModalViewController:gcvc];
        }
        
    }
    else
    {
        GKAchievementViewController* achievements = [[GKAchievementViewController alloc] init];
        if (achievements != nil)
        {
            achievements.achievementDelegate = self;
            [self launchModalViewController:achievements];
        }
    }
}

- (void) leaderboardViewControllerDidFinish : (GKLeaderboardViewController*) leaderBoard
{
    [self modalViewControllerDidFinish:leaderBoard];
}

- (void) achievementViewControllerDidFinish : (GKAchievementViewController*) achievements
{
    [self modalViewControllerDidFinish:achievements];
}

- (void) gameCenterViewControllerDidFinish : (GKGameCenterViewController *)gameCenterViewController 
{
    [self modalViewControllerDidFinish:gameCenterViewController];
}

- (void) launchModalViewController:(UIViewController *)viewController
{
    [mTopViewController presentModalViewController:viewController animated:YES];
}

- (void) modalViewControllerDidFinish:(UIViewController *)viewController
{
    [mTopViewController dismissModalViewControllerAnimated:YES];
}

@end
