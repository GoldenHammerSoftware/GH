// Copyright Golden Hammer Software
#include "GHViewInfo.h"
#include "GHCamera.h"
#include "GHMath/GHRect.h"
#include "GHMath/GHPi.h"
#include "GHRenderDevice.h"

void GHViewInfo::updateView(const GHCamera& camera, float aspectRatio, const GHRenderDevice& device, 
	GHViewInfo& leftView, GHViewInfo& rightView, bool isRenderToTexture)
{
	mIsRenderToTexture = isRenderToTexture;
	mAspectRatio = aspectRatio;

    calcViewTransform(camera.getTransform(), mEngineTransforms.mViewTransform);
    calcProjectionTransform(camera, aspectRatio, mEngineTransforms.mProjectionTransform);
	mViewportClip = camera.getViewportClip();
	calcDerivedTransforms(camera, device, isRenderToTexture);

	device.createStereoView(*this, camera, leftView, rightView, mFrustum);
	leftView.calcDerivedTransforms(camera, device, isRenderToTexture);
	rightView.calcDerivedTransforms(camera, device, isRenderToTexture);
}

void GHViewInfo::calcDerivedTransforms(const GHCamera& camera, const GHRenderDevice& device, bool isRenderToTexture)
{
	mEngineTransforms.calcDerivedTransforms(camera);

	mFrustum.createPlanes(mEngineTransforms.mViewProjTransform.getMatrix(), camera.getFarClip());
	if (camera.getIsOrtho()) {
		mFrustum.hackForOrthofView();
	}

	mEngineTransforms.mPlatformGUITrans.becomeIdentity();
	device.createDeviceViewTransforms(mEngineTransforms, mDeviceTransforms, camera, isRenderToTexture);
}

void GHViewInfo::calcProjectionTransform(const GHCamera& camera, float aspectRatio, GHTransform& outTrans)
{
	if (camera.getIsOrtho())
	{
		GHRect<float, 2> frustumRect;
		float halfCamWidth = camera.getOrthoWidth()/2.0f;
		float halfCamHeight = camera.getOrthoHeight()/2.0f;
		frustumRect.mMin.setCoords(-halfCamWidth, -halfCamHeight);
		frustumRect.mMax.setCoords(halfCamWidth, halfCamHeight);

		outTrans.orthof(frustumRect.mMin[0], frustumRect.mMax[0],
			frustumRect.mMin[1], frustumRect.mMax[1],
			camera.getNearClip(), camera.getFarClip());
	}
	else {
		GHRect<float, 2> frustumRect;
		float frustumSize = camera.getNearClip() * tanf((GHPI/180.0f)*camera.getFOV()*aspectRatio / 2.0f);
		frustumSize += camera.getFrustumFudge();
		frustumRect.mMin.setCoords(-frustumSize, -frustumSize / aspectRatio);
		frustumRect.mMax.setCoords(frustumSize, frustumSize / aspectRatio);
		
		outTrans.frustumf(frustumRect.mMin[0], frustumRect.mMax[0],
			frustumRect.mMin[1], frustumRect.mMax[1], 
			camera.getNearClip(), camera.getFarClip());
	}
}

void GHViewInfo::calcProjectionTransform(float nearClip, float farClip, const GHPoint2& fovDegrees, GHTransform& outTrans)
{
	GHRect<float, 2> frustumRect;
	float frustumSizeWidth = nearClip * tanf((GHPI / 180.0f)*fovDegrees[0] / 2.0f);
	float frustumSizeHeight= nearClip * tanf((GHPI / 180.0f)*fovDegrees[1] / 2.0f);

	//float frustumSizeWidth = fovRadians[0] / 2.0f;
	//float frustumSizeHeight = fovRadians[1] / 2.0f;

	frustumRect.mMin.setCoords(-frustumSizeWidth, -frustumSizeHeight);
	frustumRect.mMax.setCoords(frustumSizeWidth, frustumSizeHeight);

	outTrans.frustumf(frustumRect.mMin[0], frustumRect.mMax[0],
		frustumRect.mMin[1], frustumRect.mMax[1],
		nearClip, farClip);
}

void GHViewInfo::calcViewTransform(const GHTransform& cameraTrans, GHTransform& viewTransform) 
{
	viewTransform.becomeIdentity();
	
	// invert the rot via transpose.
	
	viewTransform[0] = cameraTrans[0];
	viewTransform[1] = cameraTrans[4];
	viewTransform[2] = cameraTrans[8];
	viewTransform[4] = cameraTrans[1];
	viewTransform[5] = cameraTrans[5];
	viewTransform[6] = cameraTrans[9];
	viewTransform[8] = cameraTrans[2];
	viewTransform[9] = cameraTrans[6];
	viewTransform[10] = cameraTrans[10];
	
	// do an extra rotate to face the other direction
	GHTransform rotMatrix;
	rotMatrix.becomeYRotation(GHPI);
	viewTransform.mult(rotMatrix, viewTransform);
	
	// inverse the pos.
	GHPoint3 pos;
	cameraTrans.getTranslate(pos);
	viewTransform.mult(pos, pos);
	
	viewTransform[12] = pos[0] * -1;
	viewTransform[13] = pos[1] * -1;
	viewTransform[14] = pos[2] * -1;
}

void GHViewInfo::ViewTransforms::calcDerivedTransforms(const GHCamera& camera)
{
	// view transform could differ from the main camera due to stereo.
	//viewTransforms.mViewInvTransform = camera.getTransform(); // todo: calculate this from the view transform.
	mViewInvTransform = mViewTransform;
	mViewInvTransform.invert();

	mViewTransform.mult(mProjectionTransform, mViewProjTransform);

	if (camera.getIsOrtho())
	{
		// todo: need another way to get an unproject matrix.  invert() says 0 determinants.
	}
	else
	{
		mProjectionInvTransform = mProjectionTransform;
		mProjectionInvTransform.invert();
	}

	calcBillboardTransforms(mViewInvTransform, mBillboardTransform, mBillboardUpTransform);
}

void GHViewInfo::ViewTransforms::calcBillboardTransforms(const GHTransform& viewTrans,
	GHTransform& billboardTransform, GHTransform& billboardUpTransform) const
{
	billboardUpTransform = viewTrans;
	// zero out the y rotation
	billboardUpTransform.getMatrix()[1] = 0;
	billboardUpTransform.getMatrix()[5] = 1;
	billboardUpTransform.getMatrix()[9] = 0;

	billboardTransform = viewTrans;

	billboardTransform.getMatrix()[0] *= -1.f;
	billboardTransform.getMatrix()[4] *= -1.f;
	billboardTransform.getMatrix()[8] *= -1.f;

	billboardTransform.getMatrix()[1] *= -1.f;
	billboardTransform.getMatrix()[5] *= -1.f;
	billboardTransform.getMatrix()[9] *= -1.f;

	billboardTransform.getMatrix()[2] *= -1.f;
	billboardTransform.getMatrix()[6] *= -1.f;
	billboardTransform.getMatrix()[10] *= -1.f;
}
