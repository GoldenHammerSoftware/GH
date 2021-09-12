// Copyright 2010 Golden Hammer Software
#include "CGLibrary.h"
#include "CGCard.h"
#include "CGPlayer.h"

CGLibrary::CGLibrary(void)
{
    CGCard* summonCard = new CGCard();
    summonCard->mName.setConstChars("Kya", GHString::CHT_REFERENCE);
    summonCard->mType = CGCard::CT_SUMMON;
    summonCard->mEntFile.setConstChars("kyaent.xml", GHString::CHT_REFERENCE);
    mCards.push_back(summonCard);
}

CGLibrary::~CGLibrary(void)
{
    for (int i = 0; i < mCards.size(); ++i)
    {
        delete mCards[i];
    }
}

void CGLibrary::createStarterDeck(CGPlayer& player)
{
    const int spellDeckSize = 30;
    
    int cardIdx = 0;
    for (int i = 0; i < spellDeckSize; ++i) 
    {
        player.addSpellCard(mCards[cardIdx]);
        cardIdx++;
        if (cardIdx >= mCards.size()) {
            cardIdx = 0;
        }
    }
}
