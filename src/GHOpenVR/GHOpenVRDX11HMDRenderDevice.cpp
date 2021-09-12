#include "GHOpenVRDX11HMDRenderDevice.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOpenVREyeRenderTarget.h"
#include "GHRenderDeviceDX11.h"
#include "GHCamera.h"
#include "GHMath/GHQuaternion.h"
#include "GHUtils/GHController.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHUtils/GHMessage.h"
#include "GHRenderProperties.h"
#include "GHMaterialCallbackFactoryTexture.h"
#include "GHRenderServices.h"
#include "GHMaterialCallbackMgr.h"
#include "GHTextureDX11.h"
#include "GHRenderServicesDX11.h"
#include "GHOpenVRSystem.h"
#include "GHOpenVRUtil.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderTargetFactoryDX11.h"

GHOpenVRDX11HMDRenderDevice::GHOpenVRDX11HMDRenderDevice(GHRenderServicesDX11& renderServices,
														 GHOpenVRSystem& vrSystem,
														 GHEventMgr& eventMgr)
	: mRenderServices(renderServices)
	, mEventMgr(eventMgr)
	, mVRSystem(vrSystem)
	, mGraphicsQuality(mRenderServices.getDeviceDX11()->getGraphicsQuality())
{
	initRenderTarget();
}

GHOpenVRDX11HMDRenderDevice::~GHOpenVRDX11HMDRenderDevice(void)
{
	delete mLeftRenderTarget;
	delete mRightRenderTarget;
}

void GHOpenVRDX11HMDRenderDevice::beginFrame(void)
{

}

void GHOpenVRDX11HMDRenderDevice::endFrame(void)
{
	vr::Texture_t leftEyeTexture = { mLeftRenderTarget->getTextureDX11()->getD3DTex().Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto };
	vr::EVRCompositorError error = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	if (error)
	{
		GHDebugMessage::outputString("error %u submitting left eye texture", error);
	}
	vr::Texture_t rightEyeTexture = { mRightRenderTarget->getTextureDX11()->getD3DTex().Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto };
	error = vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
	if (error)
	{
		GHDebugMessage::outputString("error %u submitting right eye texture", error);
	}
}

void GHOpenVRDX11HMDRenderDevice::createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
												   GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const
{
	leftView = mainView;
	rightView = mainView;

	GHPoint4 clip = mainView.getViewportClip();
	leftView.setViewportClip(clip);
	rightView.setViewportClip(clip);

	GHViewInfo* viewInfoArray[2];
	viewInfoArray[0] = &leftView;
	viewInfoArray[1] = &rightView;
	for (int i = 0; i < 2; ++i)
	{
		GHViewInfo::ViewTransforms& engineTransforms = viewInfoArray[i]->getEngineTransformsForModification();

		vr::HmdMatrix44_t projectionMatrix = mVRSystem.getSystem()->GetProjectionMatrix(vr::EVREye(i), camera.getNearClip(), camera.getFarClip());
		for (int col = 0; col < 4; ++col)
		{
			for (int row = 0; row < 4; ++row)
			{
				int idx = row * 4 + col;
				engineTransforms.mProjectionTransform[idx] = projectionMatrix.m[col][row];
			}
		}

		// eye offsets.
		GHTransform camTrans = camera.getTransform();
		vr::HmdMatrix34_t eyeMatrix = mVRSystem.getSystem()->GetEyeToHeadTransform(vr::EVREye(i));
		GHTransform ghEyeTransform;
		GHOpenVRUtil::convertToGHTransform(eyeMatrix, ghEyeTransform);
		if (0)
		{
			//ghEyeTransform.invert();
			//camTrans.mult(ghEyeTransform, camTrans);
		}
		else {
			GHPoint3 camPos;
			camera.getTransform().getTranslate(camPos);

			GHPoint3 eyeUnrotated;
			ghEyeTransform.getTranslate(eyeUnrotated);
			GHPoint3 eyeRotated;
			camTrans.multDir(eyeUnrotated, eyeRotated);
			camPos += eyeRotated;
			camTrans.setTranslate(camPos);
		}
		viewInfoArray[i]->calcViewTransform(camTrans, engineTransforms.mViewTransform);
	}

	{
		// pretty much a hack that works.
		// create a 90 degree fov frustum for culling.  75 is too small.  90 seems to work.
		// note: it's possible that we're really passing in fov/2 and are getting a 180 degree fov.
		// note: it's possible that we need to ask the sdk for the real fov.
		GHTransform combinedProjection;
		GHViewInfo::calcProjectionTransform(camera.getNearClip(), camera.getFarClip(), GHPoint2(90.0f, 90.0f), combinedProjection);
		GHTransform combinedViewProj;
		mainView.getEngineTransforms().mViewTransform.mult(combinedProjection, combinedViewProj);
		combinedFrustum.createPlanes(combinedViewProj.getMatrix(), camera.getFarClip());
	}
}

GHRenderTarget* GHOpenVRDX11HMDRenderDevice::getRenderTarget(RenderTargetType targetType) const
{ 
	if (targetType == GHHMDRenderDevice::RTT_LEFT)
	{	
		return mLeftRenderTarget;
	}
	if (targetType == GHHMDRenderDevice::RTT_RIGHT)
	{
		return mRightRenderTarget;
	}
	return 0;
}

bool GHOpenVRDX11HMDRenderDevice::isActive(void) const
{
	return mVRSystem.hmdIsActive();
}

void GHOpenVRDX11HMDRenderDevice::initRenderTarget(void)
{
	/*
	float backbufferMult = 2.0;
	if (mGraphicsQuality < 2)
	{
		backbufferMult = 1.5;
	}
	*/

	uint32_t width = mVRSystem.getWidth();
	uint32_t height = mVRSystem.getHeight();

	GHRenderTarget::Config rtConfig(width, height, false, true, "ovrtarget");

	GHRenderTargetDX11* leftRenderTarget = (GHRenderTargetDX11*)mRenderServices.getRenderTargetFactory()->createRenderTarget(rtConfig);
	mLeftRenderTarget = new GHOpenVREyeRenderTarget(*mRenderServices.getDevice(), leftRenderTarget);

	GHRenderTargetDX11* rightRenderTarget = (GHRenderTargetDX11*)mRenderServices.getRenderTargetFactory()->createRenderTarget(rtConfig);
	mRightRenderTarget = new GHOpenVREyeRenderTarget(*mRenderServices.getDevice(), rightRenderTarget);
}
