// Copyright Golden Hammer Software
#include "GHFrameAnimSet.h"
#include <string.h>

void GHFrameAnimSet::addSequence(const GHFrameAnimSequence& seq)
{
    mSequences.push_back(seq);
}

GHFrameAnimSequence* GHFrameAnimSet::getSequence(GHIdentifier id)
{
    for (size_t i = 0; i < mSequences.size(); ++i)
    {
        if (mSequences[i].mId == id)
        {
            return &mSequences[i];
        }
    }
    return 0;
}
