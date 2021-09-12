// Copyright 2010 Golden Hammer Software
#pragma once

#include <vector>
#include "GHString.h"
#include "GHPoint.h"
#include "GHRect.h"
#include "GHTransform.h"

class CGCard;
class GHPropertyContainer;

class CGPlayer
{
public:
    CGPlayer(GHPropertyContainer& props, const char* name, const GHPoint3& color, const GHRect<float,2> placementRegion);
    
    void startGame(void);
    // draw cards if necessary, update mana, set ui properties.
    void startTurn(void);
    
    void addSpellCard(CGCard* card) { mSpellDeck.push_back(card); }
    
    CGCard* getCard(int index);
    void removeCard(int index);
    
    const GHPoint3& getColor(void) const { return mColor; }
    const GHRect<float, 2>& getPlacementRegion(void) const { return mPlacementRegion; }

    void setCameraTrans(const GHTransform& trans) { mCameraTrans = trans; }
    const GHTransform& getCameraTrans(void) const { return mCameraTrans; }

    float getPlacementRot(void) const { return mPlacementRot; }
    void setPlacementRot(float val) { mPlacementRot = val; }
    
    void setMaxMana(int val) { mMana = val; }
    void drainMana(int val) { mMana -= val; }
    void setManaGain(int val) { mManaGain = val; }
    int getMana(void) const { return mMana; }
    
private:
    void drawSpellCard(void);
    void updateProperties(void);

private:
    GHPropertyContainer& mProps;
    GHString mName;
    int mSpellHandSize;
    GHPoint3 mColor;
    GHTransform mCameraTrans;
    float mPlacementRot;
    int mMana;
    int mMaxMana;
    int mManaGain;
    GHRect<float, 2> mPlacementRegion;
    std::vector<CGCard*> mSpellHand;
    std::vector<CGCard*> mSpellDeck;
};
