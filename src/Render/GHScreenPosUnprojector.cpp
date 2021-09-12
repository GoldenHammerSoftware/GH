// Copyright Golden Hammer Software
#include "GHScreenPosUnprojector.h"
#include "GHViewInfo.h"
#include "GHMath/GHTransform.h"
#include "GHCamera.h"
#include "GHPlatform/GHDebugMessage.h"

GHScreenPosUnprojector::GHScreenPosUnprojector(const GHViewInfo& viewInfo)
: mViewInfo(viewInfo)
{
    
}

void GHScreenPosUnprojector::unproject(const GHPoint2& screenCoords, GHPoint3& outOrigin, GHPoint3& outDir) const
{
    GHPoint2 screenPosNormalized = screenCoords;
    screenPosNormalized[1] = 1.f - screenPosNormalized[1];
    screenPosNormalized *= 2.f;
    screenPosNormalized -= GHPoint2(1,1);
    
    GHPoint3 unprojectedNear(screenPosNormalized[0], screenPosNormalized[1], 0);
    GHPoint3 unprojectedFar(screenPosNormalized[0], screenPosNormalized[1], 1);

    GHTransform projInv = mViewInfo.getEngineTransforms().mProjectionTransform;
    projInv.invert();
    
	GHTransform viewInv = mViewInfo.getEngineTransforms().mViewTransform;
    viewInv.invert();
    
    projInv.mult(unprojectedNear, unprojectedNear);
    projInv.mult(unprojectedFar, unprojectedFar);
    
    viewInv.mult(unprojectedNear, unprojectedNear);
    viewInv.mult(unprojectedFar, unprojectedFar);
     
    outOrigin = unprojectedNear;
    outDir = unprojectedFar;
    outDir -= unprojectedNear;
    outDir.normalize();
}

void GHScreenPosUnprojector::project(const GHPoint3& worldPos, GHPoint2& outScreenPos) const
{
    GHPoint3 screenPos3D;
	mViewInfo.getEngineTransforms().mViewProjTransform.mult(worldPos, screenPos3D);
    
    outScreenPos[0] = screenPos3D[0];
    outScreenPos[1] = screenPos3D[1];
    
    //un-normalize
    outScreenPos += GHPoint2(1,1);
    outScreenPos /= 2.f;
    outScreenPos[1] = 1.f - outScreenPos[1];
}
