// Copyright Golden Hammer Software
#include "GHSphere.h"
#include "stdlib.h"

GHSphere::GHSphere(void)
: mRadius(0)
, mCenter(0,0,0)
{}

GHSphere::GHSphere(const GHPoint3& center, float radius)
: mCenter(center)
, mRadius(radius)
{}

void GHSphere::toAABB(GHRect<float, 3>& aabb) const
{
    aabb.mMin = mCenter;
    aabb.mMax = mCenter;
    
    for (size_t i = 0; i < 3; ++i)
    {
        aabb.mMin[i] -= mRadius;
        aabb.mMax[i] += mRadius;
    }
}

void GHSphere::fromAABB(const GHRect<float, 3>& aabb)
{
    GHPoint3 diff = aabb.mMax;
    diff -= aabb.mMin;
    float len = diff.normalize();
    mRadius = len/2.0f;
    diff *= mRadius;
    mCenter = aabb.mMin;
    mCenter += diff;
}
