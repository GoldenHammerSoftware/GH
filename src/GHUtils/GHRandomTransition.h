// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include <vector>

// Randomly picks from a set of sub-transitions each time it is activated
// deactivate always calls deactivate on whatever was picked during activate.
class GHRandomTransition : public GHTransition
{
public:
    GHRandomTransition(void);
    ~GHRandomTransition(void);
    
    // we take ownership of child.
    void addChild(GHTransition* child, float weight);
    
    // randomly pick a child according to the weights.
    virtual void activate(void);
	virtual void deactivate(void);
    
private:
    GHTransition* pickRandomTransition(void);
    
private:
    struct ChildEntry
    {
        GHTransition* mTransition;
        float mWeight;
    };
    std::vector<ChildEntry> mChildren;
    // we pick an active transition during activate,
    //  then store it so we can deactivate it.
    GHTransition* mActiveTransition;
    // the sum of the weights of all children.
    float mWeightSum;
};
