// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHString.h"

// struct to represent one card in the deck.
class CGCard
{
public:
    enum CardType
    {
        CT_SUMMON
    };
    
public:
    GHString mName;
    GHString mEntFile;
    CardType mType;
};
