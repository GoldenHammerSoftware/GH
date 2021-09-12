// Copyright Golden Hammer Software
#pragma once

#include "GHPoint.h"
#include "GHRect.h"

class GHSphere
{
public:
    GHSphere(void);
    GHSphere(const GHPoint3& center, float radius);
    
    void toAABB(GHRect<float, 3>& aabb) const;
    void fromAABB(const GHRect<float, 3>& aabb);
    
public:
    GHPoint3 mCenter;
    float mRadius;
};
