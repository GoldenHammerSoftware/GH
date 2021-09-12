// Copyright 2010 Golden Hammer Software
#pragma once

#include <vector>

class CGCard;
class CGPlayer;

// Class to own the card instances that can exist in the game.
class CGLibrary
{
public:
    CGLibrary(void);
    ~CGLibrary(void);
    
    // possibly just a prototype-only function.
    // create a starter deck for a player.
    void createStarterDeck(CGPlayer& player);
    
private:
    std::vector<CGCard*> mCards;
};
