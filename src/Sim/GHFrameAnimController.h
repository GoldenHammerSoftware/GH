// Copyright Golden Hammer Software
#pragma once

#include "GHAnimController.h"
#include "GHFrameAnimSequence.h"
#include "GHFrameAnimBlend.h"

class GHFrameAnimSet;

// An anim controller that expects to deal with a list of frames,
//  and animations that represend cycles between frames.
class GHFrameAnimController : public GHAnimController
{
public:
    GHFrameAnimController(void);
    virtual ~GHFrameAnimController(void);
    
    virtual void setAnim(const GHIdentifier& id, float startTime=0);
    virtual void setAnimBlend(const std::vector<AnimBlend>& blends);

    void setAnimSet(GHFrameAnimSet* set);
    GHFrameAnimSet* getAnimSet(void) { return mAnimSet; }
  
    virtual AnimType getAnimType(void) const { return GHAnimController::AT_FRAME; }
    
protected:
    // info about our currently-running animation
    std::vector<GHFrameAnimBlend> mAnimBlends;
    // our set of potential animations
    GHFrameAnimSet* mAnimSet;
};
