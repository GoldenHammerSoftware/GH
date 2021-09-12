// Copyright Golden Hammer Software
#pragma once

class GHFrameAnimSequence;

// info to wrap one anim for blending.
struct GHFrameAnimBlend
{
    GHFrameAnimBlend(void)
    : mSequence(0)
    , mStartTime(0)
    , mBlendPct(1)
    {
    }
    
    const GHFrameAnimSequence* mSequence;
    float mStartTime;
    float mBlendPct;
};
