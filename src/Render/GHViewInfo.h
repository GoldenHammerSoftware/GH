// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHTransform.h"
#include "GHFrustum.h"

class GHCamera;
class GHRenderDevice;

// A class to store all the rendering transforms for a view point.
class GHViewInfo
{
public:
	struct ViewTransforms
	{
		// Assuming that stuff like ViewTransform is set, calculate the derived transforms like ViewInvTransform.
		void calcDerivedTransforms(const GHCamera& camera);

		// projection transformation.
		GHTransform mProjectionTransform;
		GHTransform mProjectionInvTransform;
		// view transform.
		GHTransform mViewTransform;
		// inverse view (camera trans)
		GHTransform mViewInvTransform;
		// view*proj
		GHTransform mViewProjTransform;
		// transform for going from 0-1 gui space to platform screen space.
		GHTransform mPlatformGUITrans;

		GHTransform mBillboardTransform;
		GHTransform mBillboardUpTransform;

	private:
		void calcBillboardTransforms(const GHTransform& viewTrans, GHTransform& billboardTransform, GHTransform& billboardUpTransform) const;
	};
	typedef enum
	{
		EYE_MONOCLE=0, // no stereo
		EYE_LEFT,
		EYE_RIGHT,
		EYE_MAX,
	} EyeIndex;

public:
    // derive the appropriate transforms from a camera.
	void updateView(const GHCamera& camera, float aspectRatio, const GHRenderDevice& device, 
		GHViewInfo& leftView, GHViewInfo& rightView, bool isRenderToTexture);

	const GHFrustum& getFrustum(void) const { return mFrustum; }
	const ViewTransforms& getEngineTransforms(void) const { return mEngineTransforms; }
	ViewTransforms& getEngineTransformsForModification(void) { return mEngineTransforms; }
	const ViewTransforms& getDeviceTransforms(void) const { return mDeviceTransforms; }
	const GHPoint4& getViewportClip(void) const { return mViewportClip; }
	float getAspectRatio(void) const { return mAspectRatio; }
	bool isRenderToTexture(void) const { return mIsRenderToTexture; }

	// called by hmdrenderdevice.
	void setViewportClip(const GHPoint4& clip) { mViewportClip = clip; }

	static void calcViewTransform(const GHTransform& cameraTrans, GHTransform& viewTransform);
	static void calcProjectionTransform(const GHCamera& camera, float aspectRatio, GHTransform& outTrans);
	static void calcProjectionTransform(float nearClip, float farClip, const GHPoint2& fovDegrees, GHTransform& outTrans); // alternate implementation

private:
	// calc any transforms that come from the view/proj transforms.
	void calcDerivedTransforms(const GHCamera& camera, const GHRenderDevice& device, bool isRenderToTexture);

private:
	GHFrustum mFrustum;
	// the view transforms for use in game code
	ViewTransforms mEngineTransforms;
	// the view transforms for use in device code.
	ViewTransforms mDeviceTransforms;
	GHPoint4 mViewportClip;
	float mAspectRatio { 1.0 };
	bool mIsRenderToTexture{ false };
};
