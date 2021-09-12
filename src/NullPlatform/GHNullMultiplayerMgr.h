// Copyright Golden Hammer Software
#pragma once

#include "GHMultiplayerMgr.h"
#include "GHMultiplayerMatchCallback.h"

class GHNullMultiplayerMgr : public GHMultiplayerMgr
{
public:
    void startMatch(void)
    {
        
    }
    
    virtual void startNextMatch(void)
    {
        
    }
    
    virtual void sendTurn(const void* data, int dataLength)
    {

    }
    
    virtual void sendGameOver(const void* data, int dataLength, const std::vector<GHMultiplayerMatchOutcome::Enum>& results)
    {
        
    }
    
    virtual const char* getLocalPlayerAlias(void) const
    {
        return "";
    }
    
    virtual void setGameCallback(GHMultiplayerMatchCallback* callback)
    {
        
    }
    
    virtual void clearCurrentMatch(void)
    {
        
    }
};
