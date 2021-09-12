// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHPlatform/GHRefCounted.h"
#include "GHString/GHIdentifier.h"
#include "GHFrameAnimSequence.h"

// a set of named anim sequences, which associate an anim with a set of frame ids.
class GHFrameAnimSet : public GHRefCounted
{
public:
    void addSequence(const GHFrameAnimSequence& seq);
    GHFrameAnimSequence* getSequence(GHIdentifier id);

    const std::vector<GHFrameAnimSequence>& getSequences(void) const { return mSequences; }
    
private:
    std::vector<GHFrameAnimSequence> mSequences;
};
