// Copyright Golden Hammer Software
#pragma once

#include <vector>

class GHString;

//An interface for the game to be informed when a multiplayer match has started.
class GHMultiplayerMatchCallback
{
public:
    virtual ~GHMultiplayerMatchCallback(void) { }
    
    virtual void matchSelectDialogDisplayed(void) = 0;
    virtual void matchReturned(bool isOurTurn, const void* data, int length, const std::vector<GHString>* playerNames) = 0;
    virtual void matchCancelled(void) = 0;
    virtual void activeMatchCountChanged(int newCount) = 0;
};
