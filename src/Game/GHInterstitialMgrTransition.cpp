// Copyright Golden Hammer Software
#include "GHInterstitialMgrTransition.h"
#include "GHMath/GHRandom.h"
#include <assert.h>
#include "GHAdTransition.h"

GHInterstitialMgrTransition::GHInterstitialMgrTransition(void)
: mActiveTransition(0)
{
}

GHInterstitialMgrTransition::~GHInterstitialMgrTransition(void)
{
	for (size_t i = 0; i < mChildren.size(); ++i)
    {
        delete mChildren[i].mTransition;
    }
}

void GHInterstitialMgrTransition::addChild(GHAdTransition* child, float weight)
{
    ChildEntry entry;
    entry.mTransition = child;
    entry.mWeight = weight;
    mChildren.push_back(entry);
}

void GHInterstitialMgrTransition::activate(void)
{
    mActiveTransition = pickRandomTransition();
    if (!mActiveTransition) return;
    mActiveTransition->activate();
}

void GHInterstitialMgrTransition::deactivate(void)
{
    if (mActiveTransition) {
        mActiveTransition->deactivate();
        mActiveTransition = 0;
    }
}

GHAdTransition* GHInterstitialMgrTransition::pickRandomTransition(void)
{
    // collect the ads that are available
    std::vector<ChildEntry> activeChildren;
    float weightSum = 0;
	for (size_t i = 0; i < mChildren.size(); ++i)
    {
        if (mChildren[i].mTransition->hasAd()) {
            activeChildren.push_back(mChildren[i]);
            weightSum += mChildren[i].mWeight;
        }
    }
    if (activeChildren.size() == 0) {
        return 0;
    }

    float randVal = GHRandom::getRandFloat(weightSum);
    float currWeight = 0;
	for (size_t i = 0; i < activeChildren.size(); ++i)
    {
        currWeight += activeChildren[i].mWeight;
        if (randVal < currWeight) {
            return activeChildren[i].mTransition;
        }
    }
    assert(false);
    return 0;
}
