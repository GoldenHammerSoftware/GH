// Copyright Golden Hammer Software
#include "GHMathUnitTests.h"
#include "GHMathDebugPrint.h"
#include "GHTransform.h"
#include "GHFloat.h"
#include "GHPi.h"
#include "GHIntersect.h"

bool GHMathUnitTests::runTests(void)
{
    bool ret = true;
    ret &= testEulerToFrom(3, 2, 1);
    ret &= testEulerToFrom(-3, -2, -1);
    ret &= testEulerToFrom(3, 0, 0);
    ret &= testEulerToFrom(0, 3, 0);
    ret &= testEulerToFrom(0, -3, 0);
    ret &= testEulerToFrom(0, 0, 3);
    ret &= testEulerToFrom(0,5,0);
    ret &= testEulerToFrom(0,-5,0);
    
    ret &= testInvert(GHTransform(0.190522f, -0.50856f, 0.839683f, 0.f,
                                  -0.839683f, -0.527537f, -0.128985f, 0.f,
                                  -0.50856f, 0.680493f, 0.527537f, 0.f,
                                  -0.525f, 1.3241f, -0.0204719f, 1.f));

    ret &= testIntersectTri(GHPoint3(0,512,0), GHPoint3(512,0,0), GHPoint3(512,512,0),
                            GHPoint3(383,522,100), GHPoint3(0,0,-1), false);
    ret &= testIntersectTri(GHPoint3(0,0,0), GHPoint3(512,0,0), GHPoint3(512,0,0),
                            GHPoint3(383,522,100), GHPoint3(0,0,-1), false);

    /*
    ret &= testIntersectTri(GHPoint3(0,0,0), GHPoint3(18,378,0), GHPoint3(21,443,0),
                            GHPoint3(54,1136,100), GHPoint3(0,0,-1), false);

    // causes D to go to 0 which causes NAN
    ret &= testIntersectTri(GHPoint3(327,514,0), GHPoint3(134,817,0), GHPoint3(76,908,0),
                            GHPoint3(434,346,100), GHPoint3(0,0,-1), false);
    ret &= testIntersectTri(GHPoint3(13,550,0), GHPoint3(72,893,0), GHPoint3(40,707,0),
                            GHPoint3(184,1551,100), GHPoint3(0,0,-1), false);

    // aren't these points inside?
    ret &= testIntersectTri(GHPoint3(0,512,0), GHPoint3(512,0,0), GHPoint3(512,512,0),
                            GHPoint3(250,5,100), GHPoint3(0,0,-1), true);
    ret &= testIntersectTri(GHPoint3(1,1,1), GHPoint3(2,1,1), GHPoint3(2,2,1),
                            GHPoint3(1.5,.1,2), GHPoint3(0,0,-1), true);
    */
    
    if (!ret) {
        GHMATHDEBUGPRINT("Math tests failed");
    }
    return ret;
}

bool GHMathUnitTests::testEulerToFrom(float testYaw, float testPitch, float testRoll)
{
    bool ret = true;
    
    GHTransform testTrans;
    testTrans.becomeYawPitchRollRotation(testYaw, testPitch, testRoll);
    float yaw, pitch, roll;
    testTrans.calcYawPitchRoll(yaw, pitch, roll);
    GHTransform newTrans;
    newTrans.becomeYawPitchRollRotation(yaw, pitch, roll);
    
    GHPoint3 testPoint(1,2,3);
    GHPoint3 newPoint(1,2,3);
    testTrans.mult(testPoint, testPoint);
    newTrans.mult(newPoint, newPoint);
    
    for (int i = 0; i < 3; ++i) {
        if (!GHFloat::isEqual(testPoint[0], newPoint[0])) {
            ret = false;
        }
    }
    if (!ret) {
        GHMATHDEBUGPRINT("Failed testEulerFromTo %f %f %f", testYaw, testPitch, testRoll);
        GHMATHDEBUGPRINT("Rot1: %f %f %f", testPoint[0], testPoint[1], testPoint[2]);
        GHMATHDEBUGPRINT("Rot2: %f %f %f", newPoint[0], newPoint[1], newPoint[2]);
        GHMATHDEBUGPRINT("NewYPR %f %f %f", yaw, pitch, roll);
    }
    return ret;
}

bool GHMathUnitTests::testInvert(const GHTransform& srcTrans)
{
    GHTransform invTrans(srcTrans);
    invTrans.invert();
    
    GHPoint3 srcPoint(3,2,1);
    GHPoint3 testPoint;
    srcTrans.mult(srcPoint, testPoint);
    invTrans.mult(testPoint, testPoint);
    
    for (int x = 0; x < 3; ++x) {
        if (!GHFloat::isEqual(testPoint[x], srcPoint[x])) {
            GHMATHDEBUGPRINT("Failed testInvert");
            return false;
        }
    }
    return true;
}

bool GHMathUnitTests::testIntersectTri(const GHPoint3& p1, const GHPoint3& p2, const GHPoint3& p3,
                                       const GHPoint3& origin, const GHPoint3& dir, bool shouldHit)
{
    bool hit = GHIntersect::intersectTri(p1, p2, p3, origin, dir, 0);
    if (hit != shouldHit) {
        GHMATHDEBUGPRINT("Failed intersectTri result, expected %d", (int)shouldHit);
        GHMATHDEBUGPRINT("Tri (%f,%f,%f) (%f,%f,%f) (%f,%f,%f)",
                                     p1[0],p1[1],p1[2],p2[0],p2[1],p2[2],p3[0],p3[1],p3[2]);
        GHMATHDEBUGPRINT("Origin(%f,%f,%f) Dir(%f,%f,%f)",
                                     origin[0],origin[1],origin[2],dir[0],dir[1],dir[2]);
        return false;
    }
    return true;
}

