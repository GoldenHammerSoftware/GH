// Copyright 2010 Golden Hammer Software
#include "CGPlayer.h"
#include "CGProperties.h"
#include "GHPropertyContainer.h"
#include "CGCard.h"

CGPlayer::CGPlayer(GHPropertyContainer& props, const char* name, const GHPoint3& color, const GHRect<float,2> placementRegion)
: mProps(props)
, mName(name, GHString::CHT_COPY)
, mSpellHandSize(5)
, mColor(color)
, mPlacementRegion(placementRegion)
, mPlacementRot(0)
, mMana(1)
, mMaxMana(1)
, mManaGain(1)
{
}

void CGPlayer::startTurn(void)
{
    mMana += mManaGain;
    if (mMana > mMaxMana) mMana = mMaxMana;
    
    if (mSpellHand.size() < mSpellHandSize) drawSpellCard();
    
    updateProperties();
}

void CGPlayer::startGame(void)
{
    mMana = mMaxMana;
    for (int i = 0; i < mSpellHandSize; ++i) {
        drawSpellCard();
    }
}

void CGPlayer::drawSpellCard(void)
{
    if (mSpellHand.size() >= mSpellHandSize) return;
    if (!mSpellDeck.size()) return;
    mSpellHand.push_back(*mSpellDeck.rbegin());
    mSpellDeck.pop_back();
}

CGCard* CGPlayer::getCard(int index)
{
    if (index > mSpellHand.size()) {
        return 0;
    }
    return mSpellHand[index];
}

void CGPlayer::removeCard(int index) 
{
    if (index <= mSpellHand.size())
    {
        std::vector<CGCard*>::iterator findIter = mSpellHand.begin();
        for (int i = 0; i < index-1; ++i)
        {
            findIter++;
        }
        mSpellHand.erase(findIter);
    }
    updateProperties();
}

void CGPlayer::updateProperties(void)
{
    mProps.setProperty(CGProperties::GP_ACTIVEPLAYERNAME, mName.getChars());
    
    if (mSpellHand.size() > 0) {
        mProps.setProperty(CGProperties::GP_CARDNAME0, mSpellHand[0]->mName.getChars());
    } else {
        mProps.setProperty(CGProperties::GP_CARDNAME0, 0);
    }
    if (mSpellHand.size() > 1) {
        mProps.setProperty(CGProperties::GP_CARDNAME1, mSpellHand[1]->mName.getChars());
    } else {
        mProps.setProperty(CGProperties::GP_CARDNAME1, 0);
    }
    if (mSpellHand.size() > 2) {
        mProps.setProperty(CGProperties::GP_CARDNAME2, mSpellHand[2]->mName.getChars());
    } else {
        mProps.setProperty(CGProperties::GP_CARDNAME2, 0);
    }
    if (mSpellHand.size() > 3) {
        mProps.setProperty(CGProperties::GP_CARDNAME3, mSpellHand[3]->mName.getChars());
    } else {
        mProps.setProperty(CGProperties::GP_CARDNAME3, 0);
    }
    if (mSpellHand.size() > 4) {
        mProps.setProperty(CGProperties::GP_CARDNAME4, mSpellHand[4]->mName.getChars());
    }
    else mProps.setProperty(CGProperties::GP_CARDNAME4, 0);
    
    mProps.setProperty(CGProperties::GP_CARDSREMAINING, (int)mSpellDeck.size());
    
    mProps.setProperty(CGProperties::GP_MANA, mMana);
    mProps.setProperty(CGProperties::GP_MAXMANA, mMaxMana);
}
