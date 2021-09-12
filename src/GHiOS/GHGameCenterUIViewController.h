// Copyright 2010 Golden Hammer Software
#pragma once
#import <UIKit/UIKit.h>
#import <GameKit/GKLeaderboardViewController.h>
#import <GameKit/GKAchievementViewController.h>

@interface GHGameCenterUIViewController : UIResponder<GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate, GKGameCenterControllerDelegate>
{
@public
    UIViewController* mTopViewController;
}

- (void) launchLeaderboard;
- (void) launchAchievements;

- (void) launchModalViewController : (UIViewController*) viewController;
- (void) modalViewControllerDidFinish : (UIViewController*) viewController;

@end
