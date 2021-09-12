// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include "GHUtils/GHController.h"
#include <vector>

class GHTimeVal;
class GHModel;

class GHAnimController : public GHController
{
public:
    enum AnimType
    {
        AT_FRAME = 0,
        AT_SKIN,
        AT_UNKNOWN,
    };
    // info to wrap one anim for blending.
    struct AnimBlend
    {
        AnimBlend(void)
        : mSequenceId(0)
        , mStartTime(0)
        , mBlendPct(1)
        {
        }
        
        GHIdentifier mSequenceId;
        float mStartTime;
        float mBlendPct;
    };
    
public:
    virtual ~GHAnimController(void) {}
    
    // setAnim and setAnimBlend are functionally equivalent and override each other.
    // setAnim is a special case with only one AnimBlend entry.
    virtual void setAnim(const GHIdentifier& id, float startTime=0) = 0;
    virtual void setAnimBlend(const std::vector<AnimBlend>& blends) = 0;
    
    virtual GHAnimController* clone(void) const = 0;
    virtual AnimType getAnimType(void) const = 0;
};
