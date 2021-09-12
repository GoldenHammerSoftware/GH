// Copyright Golden Hammer Software
#pragma once

//An interface for a game-level callback that can be
// communicated to by the platform-level in-app purchase store,
// informing the game that transactions have happened or been cancelled.
class GHIAPCallback
{
public:
    virtual ~GHIAPCallback(void) { }
    
    virtual void transactionComplete(int happened, int productID) = 0;
};
