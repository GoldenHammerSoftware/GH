// Copyright Golden Hammer Software
#include "GHCamera.h"
#include "GHMath/GHPi.h"

GHCamera::GHCamera(void)
: mFOV(80.0f)
, mNearClip(0.5f)
, mFarClip(500.0f)
, mIsOrtho(false)
, mOrthoWidth(10)
, mOrthoHeight(10)
, mViewportClip(0,0,1,1)
, mFrustumFudge(0.0f)
{
    mTransform.becomeIdentity();
}
