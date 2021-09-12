// Copyright 2010 Golden Hammer Software
#include "GHGameCenterNetworkDataMgr.h"
#import <GameKit/GameKit.h>
#import "GHGameCenterLocalPlayer.h"
#import "GHGameCenterMatchmakerDelegate.h"
#include "GHAppleIdentifiers.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHPlatform/GHDebugMessage.h"
#import <GameKit/GKMatchmakerViewController.h>
#include <algorithm>
#include "GHString/GHString.h"
#include "GHGameCenterMultiplayerUIPresenter.h"

GHGameCenterNetworkDataMgr::GHGameCenterNetworkDataMgr(GHRefCountedType<GHGameCenterLocalPlayer>* localPlayer,
                                                       GHGameCenterMultiplayerUIPresenter* uiPresenter,
                                                       GHMessageQueue& appMessageQueue)
: mLocalPlayer(localPlayer)
, mUIPresenter(uiPresenter)
, mAppMessageQueue(appMessageQueue)
, mMinPlayers(2)
, mMaxPlayers(2)
, mCurrentMatches(0)
, mWasAuthenticated(false)
{
    mLocalPlayer->acquire();
    mDelegate = [[GHGameCenterMatchmakerDelegate alloc] init];
    [mDelegate setPresenter:mUIPresenter andMultiplayerMgr:this];
    mUIPresenter->setNetworkDataMgr(this);
    localPlayer->get()->authenticate(this);
}

GHGameCenterNetworkDataMgr::~GHGameCenterNetworkDataMgr(void)
{
    [mDelegate clearCurrentMatch];
    mLocalPlayer->release();
    delete mUIPresenter;
}

void GHGameCenterNetworkDataMgr::displayMultiplayerView(void)
{
    if (!mLocalPlayer->get()->isAuthenticated()) { return; }

    launchMatchRequest(nil);
}

void GHGameCenterNetworkDataMgr::launchMatchRequest(NSArray* players)
{
    GKMatchRequest* request = [[GKMatchRequest alloc] init];
    request.recipients = players;
    request.minPlayers = mMinPlayers;
    request.maxPlayers = mMaxPlayers;
    
    GKTurnBasedMatchmakerViewController *mmvc = [[GKTurnBasedMatchmakerViewController alloc] 
                                                 initWithMatchRequest:request];    
    mmvc.turnBasedMatchmakerDelegate = mDelegate;
    mmvc.showExistingMatches = players == nil ? YES : NO;
    
    
    mUIPresenter->present(mmvc);

}

static NSData* createNSData(const void* bytes, long length)
{
    if (bytes)
    {
        return [[NSData alloc] initWithBytes:bytes length:length];
    }
    else
    {
        return [[NSData alloc] init];
    }
}


void GHGameCenterNetworkDataMgr::submitTurnToGameCenter(const void* bytes, long length)
{
    if (!mLocalPlayer->get()->isAuthenticated())
    { 
        GHDebugMessage::outputString("Error: attempting to send a turn when the GameCenter local player is not authenticated.");
        return;
    }
    
    NSData* data = createNSData(bytes, length);
    
    NSUInteger currentIndex = [mDelegate->mCurrentMatch.participants
                               indexOfObject:[mDelegate->mCurrentMatch currentParticipant]];
    NSUInteger numParticipants = [[mDelegate->mCurrentMatch participants] count];
    GKTurnBasedParticipant *nextParticipant;
    nextParticipant = [[mDelegate->mCurrentMatch participants]
                       objectAtIndex:((currentIndex + 1) % numParticipants)];
    
    [mDelegate->mCurrentMatch endTurnWithNextParticipants:@[nextParticipant]
                                   turnTimeout:GKTurnTimeoutNone
                                   matchData:data completionHandler:^(NSError *error) {
                                       if (error) 
                                       {
                                           NSLog(@"Error Submitting Turn%@", error);
                                       }
                                       else 
                                       {
                                           decrementActiveMatchCount();
                                       }
                                   }];
}

void GHGameCenterNetworkDataMgr::fillResults(const std::vector<GHMultiplayerMatchOutcome::Enum>* results)
{
    int numPlayers = (int)[[mDelegate->mCurrentMatch participants] count];
    
    if (!results)
    {
        GHDebugMessage::outputString("Error submitting end of game: There are no results. Setting all results to GKTurnBasedMatchOutcomeTied");
        for (int i = 0; i < numPlayers; ++i)
        {
            GKTurnBasedParticipant* participant = [[mDelegate->mCurrentMatch participants] objectAtIndex:i];
            [participant setMatchOutcome:GKTurnBasedMatchOutcomeTied];
        }
        return;
    }
    
    size_t numResults = results->size();
    if (numResults < numPlayers)
    {
        GHDebugMessage::outputString("Error submitting end of game. There are not enough player results. Setting remaining player results to GKTurnBasedMatchOutcomeTied");
    }
    
    for (int i = 0; i < numPlayers; ++i)
    {
        GKTurnBasedParticipant* participant = [[mDelegate->mCurrentMatch participants] objectAtIndex:i];
        if (i < numResults)
        {
            switch ((*results)[i])
            {
                case GHMultiplayerMatchOutcome::GHMULTIPLAYERMATCH_WON:
                {
                    [participant setMatchOutcome:GKTurnBasedMatchOutcomeWon];
                    break;
                }
                case GHMultiplayerMatchOutcome::GHMULTIPLAYERMATCH_LOST:
                {
                    [participant setMatchOutcome:GKTurnBasedMatchOutcomeLost];
                    break;
                }
                case GHMultiplayerMatchOutcome::GHMULTIPLAYERMATCH_TIED:
                {
                    [participant setMatchOutcome:GKTurnBasedMatchOutcomeTied];
                    break;
                }
                default:
                {
                    GHDebugMessage::outputString("Error submitting end of game: invalid player result %d. Setting result to GKTurnBasedMatchOutcomeTied", (*results)[i]);
                    [participant setMatchOutcome:GKTurnBasedMatchOutcomeTied];
                    break;
                }
            }
        }
        else 
        {
            [participant setMatchOutcome:GKTurnBasedMatchOutcomeTied];
        }
    }
}

void GHGameCenterNetworkDataMgr::submitGameEndToGameCenter(const void* bytes, long length, const std::vector<GHMultiplayerMatchOutcome::Enum>* results)
{
    if (![mDelegate itIsOurTurn])
    {
        GHDebugMessage::outputString("Error submitting end of game: it is not our turn!");
        return;
    }
    
    fillResults(results);
    
    NSData* data = createNSData(bytes, length);
    [mDelegate->mCurrentMatch endMatchInTurnWithMatchData:data 
                                        completionHandler:^(NSError *error) 
                                        {
                                            if (error)
                                            {
                                                NSLog(@"Error finishing game%@", error);
                                            }
                                            else
                                            {
                                                decrementActiveMatchCount();
                                            }
                                        }];
    
}

std::vector<GHString>* GHGameCenterNetworkDataMgr::createPlayerNameGHStrings(NSArray* players, std::vector<int>& missingPlayerIndices) const
{
    std::vector<GHString>* nameArray = new std::vector<GHString>([players count]);
    nameArray->resize([players count] + missingPlayerIndices.size());
    int offsetIntoPlayers = 0;
    for (int i = 0; i < [players count] + missingPlayerIndices.size(); ++i)
    {
        GHString& name = (*nameArray)[i];
        if (std::find(missingPlayerIndices.begin(), missingPlayerIndices.end(), i) != missingPlayerIndices.end())
        {
            name.setConstChars("Unknown", GHString::CHT_COPY);
            ++offsetIntoPlayers;
        }
        else
        {
            GKPlayer* player = [players objectAtIndex:i-offsetIntoPlayers];
            const char* alias = [[player alias] cStringUsingEncoding:NSUTF8StringEncoding];
            name.setConstChars(alias, GHString::CHT_COPY);
        }
    }
    return nameArray;
}

void GHGameCenterNetworkDataMgr::startNewGame(std::vector<GHString>* playerNames)
{
    GHPropertyContainer payload;
    continueGame(true, playerNames, 0, 0, &payload);
    calculateActiveMatchCount();
}
void GHGameCenterNetworkDataMgr::continueGame(bool isOurTurn, std::vector<GHString>* playerNames, const void* bytes, long length, GHPropertyContainer* payload)
{
    char* data = 0;
    if (length) 
    {
        //this data will be deleted in the GHGameCenter layer
        // by the message receiver. It needs to exist until then because
        // the message handling is not instantaneous.
        data = new char[length];
        memcpy(data, bytes, length);
    }
    
    GHPropertyContainer localPayload;
    if (!payload) { payload = &localPayload; }
    
    if (playerNames)
    {
        payload->setProperty(GHAppleIdentifiers::MULTIPLAYERNAMES, playerNames);
    }
    
    payload->setProperty(GHAppleIdentifiers::MULTIPLAYERMATCHDATA, data);
    payload->setProperty(GHAppleIdentifiers::MULTIPLAYERMATCHDATALENGTH, (int)length);

    payload->setProperty(GHAppleIdentifiers::MULTIPLAYERMATCHISOURTURN, isOurTurn);
    GHMessage message(GHAppleIdentifiers::MULTIPLAYERMATCHRETURNED, payload);
    mAppMessageQueue.handleMessage(message);
    calculateActiveMatchCount();
}

void GHGameCenterNetworkDataMgr::notifyMultiplayerMatchDialogPresented(void)
{
    GHPropertyContainer payload;
    GHMessage message(GHAppleIdentifiers::MULTIPLAYERMATCHDIALOGDISPLAYED, &payload);
    mAppMessageQueue.handleMessage(message);
}

void GHGameCenterNetworkDataMgr::cancelGame(void)
{
    GHPropertyContainer payload;
    GHMessage message(GHAppleIdentifiers::MULTIPLAYERMATCHCANCELLED, &payload);
    mAppMessageQueue.handleMessage(message);
}

const char* GHGameCenterNetworkDataMgr::getLocalPlayerAlias(void) const
{
    if (!mLocalPlayer->get()->isAuthenticated())
    {
        return 0;
    }

    return mLocalPlayer->get()->getAlias();
}

void GHGameCenterNetworkDataMgr::handleAuthenticationHappened(bool authenticated)
{
    if (!authenticated) return;
    if (mWasAuthenticated) return;
    mWasAuthenticated = true;
    mLocalPlayer->get()->registerLocalPlayerListener(mDelegate);
    calculateActiveMatchCount();
}

void GHGameCenterNetworkDataMgr::incrementActiveMatchCount(void)
{
    ++mCurrentMatches;
    notifyGameOfMatchCountChange();
}

void GHGameCenterNetworkDataMgr::decrementActiveMatchCount(void)
{
    --mCurrentMatches;
    notifyGameOfMatchCountChange();
}

void GHGameCenterNetworkDataMgr::notifyGameOfMatchCountChange(void)
{
    GHPropertyContainer payload;
    payload.setProperty(GHAppleIdentifiers::MULTIPLAYERMATCHCOUNT, mCurrentMatches);
    GHMessage message(GHAppleIdentifiers::MULTIPLAYERACTIVEMATCHNUMBERCHANGED, &payload);
    mAppMessageQueue.handleMessage(message);
}

void GHGameCenterNetworkDataMgr::clearCurrentMatch(void)
{
    [mDelegate clearCurrentMatch];
}

void GHGameCenterNetworkDataMgr::calculateActiveMatchCount(void)
{
    [GKTurnBasedMatch loadMatchesWithCompletionHandler:
     ^(NSArray *matches, NSError *error)
     {
         int numMatches = 0;
         for (GKTurnBasedMatch* match in matches)
         {
             if([match.currentParticipant.player.playerID isEqualToString:[GKLocalPlayer localPlayer].playerID])
             {
                 ++numMatches;
             }
         }
         mCurrentMatches = numMatches;
         notifyGameOfMatchCountChange();
     }];
}

void GHGameCenterNetworkDataMgr::loadNextMatch(void)
{
    [GKTurnBasedMatch loadMatchesWithCompletionHandler:
     ^(NSArray *matches, NSError* error)
     {
         bool matchFound = false;
         for (GKTurnBasedMatch* match in matches)
         {
             if ([[match.currentParticipant.player playerID] isEqualToString:[GKLocalPlayer localPlayer].playerID])
             {
                 matchFound = true;
                 
                 [match loadMatchDataWithCompletionHandler:
                  ^(NSData* matchData, NSError* matchDataError)
                  {
                     if (matchDataError)
                     {
                         NSLog(@"Error loading match data: %@", matchDataError.description);
                     }
                     
                     startMatch(match, matchData);
                  }];
                 break;
             }
         }
         if (!matchFound)
         {
            cancelGame();
         }
     }];
}

void GHGameCenterNetworkDataMgr::startMatch(GKTurnBasedMatch* match)
{
    startMatch(match, [match matchData]);
}

void GHGameCenterNetworkDataMgr::startMatch(GKTurnBasedMatch* match, NSData* matchData)
{
    [mDelegate setCurrentMatch:match];
    
    NSMutableArray* playerIDArray = [[NSMutableArray alloc] init];
    for (int i = 0; i < [match.participants count]; ++i)
    {
        GKTurnBasedParticipant* player = [match.participants objectAtIndex:i];
        NSString* playerID = [[player player] playerID];
        if (playerID != nil)
        {
            [playerIDArray addObject:playerID];
        }
    }
    
    [GKPlayer loadPlayersForIdentifiers:playerIDArray withCompletionHandler:
    ^(NSArray *players, NSError* error)
    {
        if (error)
        {
            NSLog(@"Error loading players: %@", error.description);
        }
        
        std::vector<int> missingPlayerIndices;
        for (int i = 0; i < [[match participants] count]; ++i)
        {
            GKTurnBasedParticipant* participant = [[match participants] objectAtIndex:i];
            bool isKnownPlayer = false;
            for (GKPlayer* player in players)
            {
                if ([[[participant player] playerID] isEqualToString:[player playerID]])
                {
                    isKnownPlayer = true;
                    break;
                }
            }
            if (!isKnownPlayer)
            {
                missingPlayerIndices.push_back(i);
            }
        }
        
        std::vector<GHString>* ghStringPlayernames = createPlayerNameGHStrings(players, missingPlayerIndices);
        
        GKTurnBasedParticipant *firstParticipant = [match.participants objectAtIndex:0];
        if (firstParticipant.lastTurnDate == NULL)
        {
            // It's a new game!
            startNewGame(ghStringPlayernames);
        }
        else
        {
            bool isOurTurn = [mDelegate itIsOurTurn];
            continueGame(isOurTurn, ghStringPlayernames, [matchData bytes], [matchData length], 0);
        }
    }];
}

