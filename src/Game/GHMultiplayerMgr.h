// Copyright Golden Hammer Software
#pragma once

#include "GHMultiplayerMatchOutcome.h"
#include <vector>

class GHMultiplayerMatchCallback;

//Interface to the game to manage finding and starting multiplayer games.
class GHMultiplayerMgr
{
public:
    virtual ~GHMultiplayerMgr(void) { }
    virtual void startMatch(void) = 0;
    virtual void startNextMatch(void) = 0;
    virtual void sendTurn(const void* data, int dataLength) = 0;
    virtual void sendGameOver(const void* data, int dataLength, const std::vector<GHMultiplayerMatchOutcome::Enum>& results) = 0;
    virtual const char* getLocalPlayerAlias(void) const = 0;
    virtual void clearCurrentMatch(void) = 0;
    
    virtual void setGameCallback(GHMultiplayerMatchCallback* callback) = 0;
};
