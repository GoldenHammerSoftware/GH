// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include <vector>

class GHAdTransition;

// Randomly picks from a set of sub-transitions each time it is activated
// deactivate always calls deactivate on whatever was picked during activate.
class GHInterstitialMgrTransition : public GHTransition
{
public:
    GHInterstitialMgrTransition(void);
    ~GHInterstitialMgrTransition(void);
    
    // we take ownership of child.
    void addChild(GHAdTransition* child, float weight);
    
    // randomly pick a child according to the weights.
    virtual void activate(void);
	virtual void deactivate(void);
    
private:
    GHAdTransition* pickRandomTransition(void);
    
private:
    struct ChildEntry
    {
        GHAdTransition* mTransition;
        float mWeight;
    };
    std::vector<ChildEntry> mChildren;
    // we pick an active transition during activate,
    //  then store it so we can deactivate it.
    GHAdTransition* mActiveTransition;
};
