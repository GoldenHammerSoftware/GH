// Copyright 2010 Golden Hammer Software
#pragma once

//#import <UIKit/UIKit.h>
#import "GHGameCenterMatchmakerDelegate.h"
#include "GHPlatform/GHRefCounted.h"
#include <vector>
#include "GHMultiplayerMatchOutcome.h"

class GHGameCenterLocalPlayer;
class GHMessageQueue;
class GHMultiplayerMatchCallback;
class GHPropertyContainer;
class GHGameCenterMultiplayerUIPresenter;
class GHString;

//This class handles communication directly with the GameCenter calls,
// including gathering the network data and presenting the GameKit UIs.
// It operates in the UI thread, passing data back to the game thread via message queue.
class GHGameCenterNetworkDataMgr
{
public:
    GHGameCenterNetworkDataMgr(GHRefCountedType<GHGameCenterLocalPlayer>* localPlayer,
                               GHGameCenterMultiplayerUIPresenter* uiPresenter,
                               GHMessageQueue& appMessageQueue);
    
    ~GHGameCenterNetworkDataMgr(void);

    void displayMultiplayerView(void);
    
    std::vector<GHString>* createPlayerNameGHStrings(NSArray* players, std::vector<int>& missingPlayerIndices) const;
    void startNewGame(std::vector<GHString>* playerNames);
    void continueGame(bool isOurTurn, std::vector<GHString>* playerNames, const void* bytes, long length, GHPropertyContainer* payload);
    void notifyMultiplayerMatchDialogPresented(void);
    void cancelGame(void);
    void submitTurnToGameCenter(const void* bytes, long length);
    void submitGameEndToGameCenter(const void* bytes, long length, const std::vector<GHMultiplayerMatchOutcome::Enum>* results);
    void clearCurrentMatch(void);
    
    const char* getLocalPlayerAlias(void) const;
    
    void launchMatchRequest(NSArray* players);
    
    void handleAuthenticationHappened(bool authenticated);
    
    void incrementActiveMatchCount(void);
    void decrementActiveMatchCount(void);
    
    void calculateActiveMatchCount(void);
    
    void loadNextMatch(void);
    void startMatch(GKTurnBasedMatch* match);
    void startMatch(GKTurnBasedMatch* match, NSData* matchData);
    
private:
    void notifyGameOfMatchCountChange(void);
    void fillResults(const std::vector<GHMultiplayerMatchOutcome::Enum>* results);
    
private:
    GHRefCountedType<GHGameCenterLocalPlayer>* mLocalPlayer;
    GHGameCenterMultiplayerUIPresenter* mUIPresenter;
    GHMessageQueue& mAppMessageQueue;
    GHGameCenterMatchmakerDelegate* mDelegate;
    int mMinPlayers;
    int mMaxPlayers;
    bool mWasAuthenticated;
    
    int mCurrentMatches;
};

