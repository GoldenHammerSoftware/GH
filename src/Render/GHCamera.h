// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHTransform.h"

// Describes a viewpoint in the world.
class GHCamera
{
public:
    GHCamera(void);
    
    const GHTransform& getTransform(void) const { return mTransform; }
    GHTransform& getTransform(void) { return mTransform; }
    
    void setNearClip(float val) { mNearClip = val; }
    const float getNearClip(void) const { return mNearClip; }

	void setFarClip(float val) { mFarClip = val; }
	const float getFarClip(void) const { return mFarClip; }

    // degrees of width visible.
	void setFOV(float val) { mFOV = val; }
	const float getFOV(void) const { return mFOV; }

	void setIsOrtho(bool val) { mIsOrtho = val; }
    const bool getIsOrtho(void) const { return mIsOrtho; }

	void setOrthoWidth(float val) { mOrthoWidth = val; }
	const float getOrthoWidth(void) const { return mOrthoWidth; }

	void setOrthoHeight(float val) { mOrthoHeight = val; }
	const float getOrthoHeight(void) const { return mOrthoHeight; }

	void setViewportClip(const GHPoint4& newClip) { mViewportClip = newClip; }
	const GHPoint4& getViewportClip(void) const { return mViewportClip; }

	void setFrustumFudge(float val) { mFrustumFudge = val; }
	float getFrustumFudge(void) const { return mFrustumFudge; }

private:
    GHTransform mTransform;
    
    // distance of the near clip plane.
	float mNearClip;
	// distance of the far clip plane.
	float mFarClip;
	// field of view in radians.
	float mFOV;
	// fudge factor for making the frustum bigger.
	float mFrustumFudge;

	// restrict the rendered portion of the backbuffer
	GHPoint4 mViewportClip;

	// values for otho projection
	bool mIsOrtho;
	float mOrthoWidth;
	float mOrthoHeight;
};
