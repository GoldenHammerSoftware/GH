// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMultiplayerMgr.h"
#include <vector>

class GHString;
class GHMessageQueue;
class GHGameCenterMultiplayerMessageHandler;
class GHEventMgr;
class GHGameCenterNetworkDataMgr;
class GHPropertyContainer;
class GHMultiplayerMatchCallback;

//This class handles interfacing between GHGameCenter and the game.
// It operates in the Game thread, and communicates to the UI thread with
// a message queue.

//Since this is the class that the game level has a direct pointer to 
// (through the GHMultiplayerMgr interface), it is also in charge
// of owning the other parts of GHGameCenterMultiplayer,
// which includes the message handler and the UI thread interface
class GHGameCenterMultiplayerMgr : public GHMultiplayerMgr
{
public:
    GHGameCenterMultiplayerMgr(GHMessageQueue& uiMessageQueue,
                               GHEventMgr& uiThreadEventMgr,
                               GHEventMgr& gameThreadEventMgr,
                               GHGameCenterNetworkDataMgr* networkDataMgr);
    ~GHGameCenterMultiplayerMgr(void);
    
    //Called from the game. This should request that GameCenter match selection UI gets brought up
    virtual void startMatch(void);
    //opens list of matches, starts the next available one
    virtual void startNextMatch(void);
    virtual void sendTurn(const void* data, int dataLength);
    virtual void sendGameOver(const void* data, int dataLength, const std::vector<GHMultiplayerMatchOutcome::Enum>& results);
    virtual void clearCurrentMatch(void);
    
    virtual const char* getLocalPlayerAlias(void) const;
    
    void handleMatchSelectDialogPresented(void);
    void handleMatchReturnedFromGC(bool isOurTurn, void* data, int length, const std::vector<GHString>* playerNames);
    void handleMatchCancelledByGC(void);
    void handleActiveMatchCountChanged(int activeMatchCount);
    
    virtual void setGameCallback(GHMultiplayerMatchCallback* callback);

private:
    void sendTurn(const void* data, int datalength, GHPropertyContainer& payload);
    
private:
    GHMessageQueue& mUIMessageQueue;
    GHGameCenterMultiplayerMessageHandler* mMessageHandler;
    GHGameCenterNetworkDataMgr* mNetworkDataMgr;
    GHMultiplayerMatchCallback* mGameCallback;
    
    int mActiveMatchCountToReport;
};
