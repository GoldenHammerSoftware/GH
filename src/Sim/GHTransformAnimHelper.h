// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHFrameAnimBlend.h"
#include <vector>

class GHTimeVal;
class GHTransformAnimData;

// A place for shared code between GHTransformAnimController and GHSkeletonAnimController
class GHTransformAnimHelper
{
public:
    bool blendTransform(std::vector<GHFrameAnimBlend>& blends, const GHTimeVal& time,
                        const GHTransformAnimData& animData, GHPoint16& ret) const;
    void blendAnimFrames(const GHPoint16& lowMatrix, const GHPoint16& highMatrix, float pctLow, GHPoint16& matrixRes) const;
};
