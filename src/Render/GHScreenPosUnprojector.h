// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

class GHViewInfo;

// Takes a screen pos and turns it into a world space ray.
class GHScreenPosUnprojector
{
public:
    GHScreenPosUnprojector(const GHViewInfo& viewInfo);
    
    //ScreenPos should be in 0-1 screenspace
    void unproject(const GHPoint2& screenPos, GHPoint3& outOrigin, GHPoint3& outDir) const;
    
    void project(const GHPoint3& worldPos, GHPoint2& outScreenPos) const;
    
private:
    const GHViewInfo& mViewInfo;
};
