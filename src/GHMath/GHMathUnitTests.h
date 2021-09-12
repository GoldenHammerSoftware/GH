// Copyright Golden Hammer Software
#pragma once

#include "GHPoint.h"

class GHTransform;

// dumping ground for testing out the math library.
class GHMathUnitTests
{
public:
    static bool runTests(void);
    static bool testEulerToFrom(float testYaw, float testPitch, float testRoll);
    static bool testInvert(const GHTransform& srcTrans);
    static bool testIntersectTri(const GHPoint3& p1, const GHPoint3& p2, const GHPoint3& p3,
                                 const GHPoint3& origin, const GHPoint3& dir, bool shouldHit);
};
