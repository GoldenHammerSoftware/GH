// Copyright Golden Hammer Software
#include "GHRandomTransition.h"
#include "GHMath/GHRandom.h"
#include <assert.h>

GHRandomTransition::GHRandomTransition(void)
: mActiveTransition(0)
, mWeightSum(0)
{
}

GHRandomTransition::~GHRandomTransition(void)
{
    for (size_t i = 0; i < mChildren.size(); ++i)
    {
        delete mChildren[i].mTransition;
    }
}

void GHRandomTransition::addChild(GHTransition* child, float weight)
{
    mWeightSum += weight;
    ChildEntry entry;
    entry.mTransition = child;
    entry.mWeight = weight;
    mChildren.push_back(entry);
}

void GHRandomTransition::activate(void)
{
    mActiveTransition = pickRandomTransition();
    if (!mActiveTransition) return;
    mActiveTransition->activate();
}

void GHRandomTransition::deactivate(void)
{
    if (mActiveTransition) {
        mActiveTransition->deactivate();
        mActiveTransition = 0;
    }
}

GHTransition* GHRandomTransition::pickRandomTransition(void)
{
    float randVal = GHRandom::getRandFloat(mWeightSum);
    float currWeight = 0;
    for (size_t i = 0; i < mChildren.size(); ++i)
    {
        currWeight += mChildren[i].mWeight;
        if (randVal < currWeight) {
            return mChildren[i].mTransition;
        }
    }
    assert(false);
    return 0;
}
