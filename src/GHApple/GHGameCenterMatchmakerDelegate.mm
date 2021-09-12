// Copyright 2010 Golden Hammer Software
#import "GHGameCenterMatchmakerDelegate.h"
#include "GHGameCenterNetworkDataMgr.h"
#include <vector>
#include "GHGameCenterMultiplayerUIPresenter.h"

@implementation GHGameCenterMatchmakerDelegate

- (void) setPresenter:(GHGameCenterMultiplayerUIPresenter*)uiPresenter andMultiplayerMgr:(GHGameCenterNetworkDataMgr*)multiplayerMgr
{
    mUIPresenter = uiPresenter;
    mMultiplayerMgr = multiplayerMgr;
    mCurrentMatch = 0;
}

- (void) clearCurrentMatch
{
    mCurrentMatch = 0;
}

- (void) setCurrentMatch:(GKTurnBasedMatch *)match
{
    [self clearCurrentMatch];
    mCurrentMatch = match;
}

- (bool) itIsOurTurn
{
    if (mCurrentMatch == nil) { return NO; }
    return [[mCurrentMatch.currentParticipant.player playerID] isEqualToString:[GKLocalPlayer localPlayer].playerID];
}

- (void) turnBasedMatchmakerViewController: (GKTurnBasedMatchmakerViewController *)viewController 
                                             didFindMatch:(GKTurnBasedMatch *)match 
{
    mUIPresenter->dismiss();
    
    if(mMultiplayerMgr)
    {
        mMultiplayerMgr->startMatch(match);
    }
}

-(void)turnBasedMatchmakerViewControllerWasCancelled: (GKTurnBasedMatchmakerViewController *)viewController 
{
    mUIPresenter->dismiss();
    if (mMultiplayerMgr)
    {
        mMultiplayerMgr->calculateActiveMatchCount();
        mMultiplayerMgr->cancelGame();
    }
}

-(void)turnBasedMatchmakerViewController: (GKTurnBasedMatchmakerViewController *)viewController
                                            didFailWithError:(NSError *)error 
{
    mUIPresenter->dismiss();
    if (mMultiplayerMgr)
    {
        mMultiplayerMgr->cancelGame();
    }
    NSLog(@"Error finding match: %@", error.localizedDescription);
}

-(void)turnBasedMatchmakerViewController: (GKTurnBasedMatchmakerViewController *)viewController 
                                            playerQuitForMatch:(GKTurnBasedMatch *)match 
{
    NSUInteger currentPlayer = [match.participants indexOfObject:match.currentParticipant];
    
    [match.currentParticipant setMatchOutcome:GKTurnBasedMatchOutcomeQuit];
    
    GKTurnBasedParticipant* nextPlayer = nil;
    int numPlayers = 0;
    for (int i = 0; i < [match.participants count]; ++i)
    {
        GKTurnBasedParticipant* player = [match.participants objectAtIndex:(currentPlayer+i+1)%match.participants.count];
        if (player.matchOutcome != GKTurnBasedMatchOutcomeQuit)
        {
            ++numPlayers;
            if (!nextPlayer)
            {
                nextPlayer = player;
            }
        }
    }
    
    if (numPlayers > 1)
    {
        if ([[match.currentParticipant.player playerID] isEqualToString:[GKLocalPlayer localPlayer].playerID])
        {
            [match participantQuitInTurnWithOutcome:GKTurnBasedMatchOutcomeQuit 
                                    nextParticipants:@[nextPlayer]
                                         turnTimeout:GKTurnTimeoutNone
                                          matchData:match.matchData 
                                  completionHandler:nil];
        }
        else 
        {
            [match participantQuitOutOfTurnWithOutcome:GKTurnBasedMatchOutcomeQuit
                                 withCompletionHandler:nil];

        }
    }
    else //this is the end of the game
    {
        for (int i = 0; i < [match.participants count]; ++i)
        {
            GKTurnBasedParticipant* player = [match.participants objectAtIndex:i];

            if (player.matchOutcome == GKTurnBasedMatchOutcomeNone)
            {
                [player setMatchOutcome:GKTurnBasedMatchOutcomeWon];
            }
        }
        
        if ([[match.currentParticipant.player playerID] isEqualToString:[GKLocalPlayer localPlayer].playerID])
        { 
            [match endMatchInTurnWithMatchData:match.matchData
                                        completionHandler:nil];
        }
        else 
        {
            [match participantQuitOutOfTurnWithOutcome:GKTurnBasedMatchOutcomeQuit 
                                 withCompletionHandler:nil];
        }
        
    }

    if (mMultiplayerMgr) { mMultiplayerMgr->decrementActiveMatchCount(); }
}

//GKTurnBasedEventListener stuff
- (void)player:(GKPlayer *)player didRequestMatchWithRecipients:(NSArray<GKPlayer *> *)recipientPlayers;
{
    //Called when our local player creates an invite in the GameCenter app
    mUIPresenter->dismiss();
    mMultiplayerMgr->launchMatchRequest(recipientPlayers);
}

- (void)player:(GKPlayer *)player didRequestMatchWithOtherPlayers:(NSArray<GKPlayer *> *)playersToInvite
{
    mUIPresenter->dismiss();
    mMultiplayerMgr->launchMatchRequest(playersToInvite);
}

- (void)player:(GKPlayer *)player didAcceptInvite:(GKInvite *)invite
{
    int breakpoint = 6;
    breakpoint = 7;
}

- (void)player:(GKPlayer *)currentPlayer receivedTurnEventForMatch:(GKTurnBasedMatch *)match didBecomeActive:(BOOL)didBecomeActive;
{
    //called when other player declines invitation
    //called when other player quits
    //called when I accept an invitation
    //called when other player finishes a turn
    
    //This method is called when the it becomes the player’s turn. It is also called when any of the following events happen:
    //    The current turn has a time-out associated with it and the turn is about to expire.
    //    Player accepts an invite from another player.
    //    Turn was passed to another player. In this case, didBecomeActive is false.
    //    Match data is saved by another player.
    //    Player receives a reminder.
    
    int numPlayersQuit = 0;
    for (GKTurnBasedParticipant* participant in [match participants])
    {
        GKTurnBasedMatchOutcome outcome = [participant matchOutcome];
        GKTurnBasedParticipantStatus status = [participant status];
        if (outcome == GKTurnBasedMatchOutcomeQuit
            || status == GKTurnBasedParticipantStatusDeclined)
        {
            ++numPlayersQuit;
        }
    }
    bool gameIsOver = (numPlayersQuit >= ([[match participants] count] - 1));
    
    if ([match status] == GKTurnBasedMatchStatusEnded)
    {
        gameIsOver = true;
    }

        
    //This is the last played match this session, and the player is possibly in-game.
    // uncomment to enable the autoplay-current-game feature
//    if (!gameIsOver)
//    {
//        [self reloadCurrentMatch:match];
//    }
    
    if ([[match.currentParticipant.player playerID] isEqualToString:[GKLocalPlayer localPlayer].playerID])
    {
        if (gameIsOver && [match status] != GKTurnBasedMatchStatusEnded)
        {
            [[match currentParticipant] setMatchOutcome:GKTurnBasedMatchOutcomeWon];
            [match endMatchInTurnWithMatchData:[match matchData] completionHandler:nil];
        }
    }
    mMultiplayerMgr->calculateActiveMatchCount();
}

- (void)player:(GKPlayer *)player wantsToQuitMatch:(GKTurnBasedMatch *)match
{
    //This is called when any player forfeits a match.
    //Other callbacks will handle everything here that needs to be handled (eg, ending the game on both ends)
    // There's nothing additional we need to do in this callback.
    
    int breakpoint =6 ;
    breakpoint = 7;
}

- (void)player:(GKPlayer *)player matchEnded:(GKTurnBasedMatch *)match;
{
    [self reloadCurrentMatch:match];
    mMultiplayerMgr->calculateActiveMatchCount();
}

-(void)reloadCurrentMatch:(GKTurnBasedMatch *)match
{
    if (mCurrentMatch && [match.matchID isEqualToString:mCurrentMatch.matchID])
    {
        mCurrentMatch = match;
        
        
        if(mMultiplayerMgr)
        {
            mMultiplayerMgr->startMatch(match);
        }
    }
}


@end
