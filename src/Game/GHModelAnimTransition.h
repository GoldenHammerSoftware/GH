// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHIdentifier.h"
#include <vector>

class GHModel;
class GHTimeVal;

// A class to play an animation on entering an entity state.
class GHModelAnimTransition : public GHTransition
{
public:
    // construct with at least one anim.
    GHModelAnimTransition(GHModel& model, const GHIdentifier& anim, const GHTimeVal& time, float timeRandom);
    // add an anim to the potential list.
    void addAnim(const GHIdentifier& anim) { mAnims.push_back(anim); }
    
    virtual void activate(void);
	virtual void deactivate(void) {}

private:
    GHModel& mModel;
    const GHTimeVal& mTime;
    float mTimeRandom;
    // a list of anims to pick from on activate.
    // we guarantee at least one entry.
    std::vector<GHIdentifier> mAnims;
};
