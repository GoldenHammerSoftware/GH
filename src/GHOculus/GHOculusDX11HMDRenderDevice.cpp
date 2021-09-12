#include "GHOculusDX11HMDRenderDevice.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOculusDX11RenderTarget.h"
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
#include "GHOculusSystemController.h"
#include "GHOculusUtil.h"
#include "GHUtils/GHEventMgr.h"

GHOculusDX11HMDRenderDevice::GHOculusDX11HMDRenderDevice(GHRenderServicesDX11& renderServices,
	const GHOculusSystemController& oculusSystem,
	GHEventMgr& eventMgr)
	: mRenderServices(renderServices)
	, mOculusSystem(oculusSystem)
	, mGraphicsQuality(mRenderServices.getDeviceDX11()->getGraphicsQuality())
	, mMessageHandler(*this, eventMgr)
{
	createMirrorTexture();

	ovrSizei leftSize = ovr_GetFovTextureSize(mOculusSystem.getSession(), ovrEye_Left, mOculusSystem.getHMDDesc().DefaultEyeFov[ovrEye_Left], 2.f);
	ovrSizei rightSize = ovr_GetFovTextureSize(mOculusSystem.getSession(), ovrEye_Right, mOculusSystem.getHMDDesc().DefaultEyeFov[ovrEye_Right], 2.f);
	mAspectRatio = ((float)leftSize.w)/((float)leftSize.h);

	initRenderTarget();
}

GHOculusDX11HMDRenderDevice::~GHOculusDX11HMDRenderDevice(void)
{
	if (mRenderTarget)
	{
		delete mRenderTarget;
	}

	if (mMirrorTexture)
	{
		ovr_DestroyMirrorTexture(mOculusSystem.getSession(), mMirrorTexture);
		mMirrorGHTexture->release();
	}
}

const ovrTrackingState& GHOculusDX11HMDRenderDevice::getTrackingState(void)
{
	return mOculusSystem.getTrackingState();
}

void GHOculusDX11HMDRenderDevice::beginFrame(void)
{
}

float GHOculusDX11HMDRenderDevice::getOculusToGameScale(void) const
{
	return mOculusSystem.getOculusToGameScale();
}

void GHOculusDX11HMDRenderDevice::endFrame(void)
{
	if (!mOculusSystem.isVisible())
	{
		return;
	}

	mRenderTarget->commitChanges();

	ovrPosef hmdToEyeOffset[ovrEye_Count];
	hmdToEyeOffset[ovrEye_Left] = mOculusSystem.getEyeRenderDesc(ovrEye_Left).HmdToEyePose;
	hmdToEyeOffset[ovrEye_Right] = mOculusSystem.getEyeRenderDesc(ovrEye_Right).HmdToEyePose;

	//ovrPosef         eyeRenderPose[2];
	//double sensorSampleTime;    // sensorSampleTime is fed into the layer later
	//ovr_GetEyePoses(mSession, mFrameIndex, ovrTrue, hmdToEyeOffset, eyeRenderPose, &sensorSampleTime);
	double sensorSampleTime = ovr_GetPredictedDisplayTime(mOculusSystem.getSession(), 0);

	ovrLayerEyeFov layer = {};

	layer.Header.Type = ovrLayerType_EyeFov;
	layer.Header.Flags = 0;
	layer.ColorTexture[ovrEye_Left] = mRenderTarget->getSwapChain();
	layer.ColorTexture[ovrEye_Right] = mRenderTarget->getSwapChain();
	layer.Viewport[ovrEye_Left] = mRenderTarget->getViewport(ovrEye_Left);
	layer.Viewport[ovrEye_Right] = mRenderTarget->getViewport(ovrEye_Right);

	for (int eye = 0; eye < 2; ++eye)
	{
		layer.Fov[eye] = mOculusSystem.getHMDDesc().DefaultEyeFov[eye];
		layer.RenderPose[eye] = mOculusSystem.getEyeRenderPose((ovrEyeType)eye);
		layer.SensorSampleTime = sensorSampleTime;
	}

	ovrLayerHeader*				overLayerHeaderPtr = &layer.Header;

	ovrViewScaleDesc viewScaleDesc;
	viewScaleDesc.HmdSpaceToWorldScaleInMeters = mOculusSystem.getGameToOculusScale();
	viewScaleDesc.HmdToEyePose[0] = hmdToEyeOffset[0];
	viewScaleDesc.HmdToEyePose[1] = hmdToEyeOffset[1];

	ovrResult presentResult = ovr_SubmitFrame(mOculusSystem.getSession(), mFrameIndex, &viewScaleDesc, &overLayerHeaderPtr, 1);
	bool success = GHOculusUtil::checkOvrResult(presentResult, "ovr_SubmitFrame");

	++mFrameIndex;
}

void GHOculusDX11HMDRenderDevice::createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
	GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const
{
	if (!mOculusSystem.isVisible())
	{
		return;
	}

	leftView = mainView;
	rightView = mainView;
	GHPoint4 clip = mainView.getViewportClip();
	float clipWidth = clip[2] - clip[0];
	
	GHPoint4 leftClip = clip;
	leftClip[2] = leftClip[0] + clipWidth / 2.0f;
	leftView.setViewportClip(leftClip);
	
	GHPoint4 rightClip = clip;
	rightClip[0] = clip[0] + clipWidth / 2.0f;
	rightClip[2] = rightClip[0] + clipWidth / 2.0f;
	rightView.setViewportClip(rightClip);

	ovrEyeRenderDesc eyeRenderDesc[2];
	eyeRenderDesc[0] = ovr_GetRenderDesc(mOculusSystem.getSession(), ovrEye_Left, mOculusSystem.getHMDDesc().DefaultEyeFov[0]);
	eyeRenderDesc[1] = ovr_GetRenderDesc(mOculusSystem.getSession(), ovrEye_Right, mOculusSystem.getHMDDesc().DefaultEyeFov[1]);

	ovrPosef HmdToEyeOffset[2] = { eyeRenderDesc[0].HmdToEyePose, eyeRenderDesc[1].HmdToEyePose };

	//double sensorSampleTime;    // sensorSampleTime is fed into the layer later
	//ovr_GetEyePoses(mSession, mFrameIndex, ovrTrue, HmdToEyeOffset, EyeRenderPose, &sensorSampleTime);

	GHViewInfo* viewInfoArray[2];
	viewInfoArray[0] = &leftView;
	viewInfoArray[1] = &rightView;
	for (int i = 0; i < 2; ++i)
	{
		GHViewInfo::ViewTransforms& engineTransforms = viewInfoArray[i]->getEngineTransformsForModification();

		ovrMatrix4f ovrproj = ovrMatrix4f_Projection(eyeRenderDesc[i].Fov, camera.getNearClip(), camera.getFarClip(), ovrProjection_None);
		for (int col = 0; col < 4; ++col)
		{
			for (int row = 0; row < 4; ++row)
			{
				int idx = row * 4 + col;
				engineTransforms.mProjectionTransform.getMatrix()[idx] = ovrproj.M[col][row];
			}
		}

		// eye offsets.
		GHTransform camTrans = camera.getTransform();
		GHPoint3 camPos;
		camera.getTransform().getTranslate(camPos);

		GHPoint3 eyeUnrotated;
		eyeUnrotated[0] = -eyeRenderDesc[i].HmdToEyePose.Position.x;
		eyeUnrotated[1] = eyeRenderDesc[i].HmdToEyePose.Position.y;
		eyeUnrotated[2] = -eyeRenderDesc[i].HmdToEyePose.Position.z;
		eyeUnrotated *= mOculusSystem.getOculusToGameScale();
		GHPoint3 eyeRotated;
		camTrans.multDir(eyeUnrotated, eyeRotated);
		camPos += eyeRotated;
		camTrans.setTranslate(camPos);

		// is eye orientation needed? doesn't look like it.
		//mEyeRenderPose[i].Orientation;

		viewInfoArray[i]->calcViewTransform(camTrans, engineTransforms.mViewTransform);
	}

	{
		// pretty much a hack that works.
		// create a 90 degree fov frustum for culling.  75 is too small.  90 seems to work.
		// note: it's possible that we're really passing in fov/2 and are getting a 180 degree fov.
		// note: it's possible that we need to ask the oculus sdk for the real fov.
		GHTransform combinedProjection;
		GHViewInfo::calcProjectionTransform(camera.getNearClip(), camera.getFarClip(), GHPoint2(90.0f, 90.0f), combinedProjection);
		GHTransform combinedViewProj;
		mainView.getEngineTransforms().mViewTransform.mult(combinedProjection, combinedViewProj);
		combinedFrustum.createPlanes(combinedViewProj.getMatrix(), camera.getFarClip());
	}
}

GHRenderTarget* GHOculusDX11HMDRenderDevice::getRenderTarget(RenderTargetType targetType) const
{ 
	if (targetType == GHHMDRenderDevice::RTT_BOTH)
	{
		return mRenderTarget;
	}
	return 0;
}

bool GHOculusDX11HMDRenderDevice::isActive(void) const
{
	return mOculusSystem.isVisible();
}

void GHOculusDX11HMDRenderDevice::createMirrorTexture(void)
{
	//constants from example code (Win32_BasicVR.h)
	float scaleWindowW = .25f;
	float scaleWindowH = .25f;
	float scaleMirrorW = 1.f;
	float scaleMirrorH = 1.f;

	ovrMirrorTextureDesc mirrorTextureDesc = {};
	mirrorTextureDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	mirrorTextureDesc.Width = int(scaleMirrorW * (scaleWindowW * mOculusSystem.getHMDDesc().Resolution.w)); //DIRECTX.WinSizeW
	mirrorTextureDesc.Height = int(scaleMirrorH * (scaleWindowH * mOculusSystem.getHMDDesc().Resolution.h)); //DIRECTX.WinSizeH

	ovrResult result = ovr_CreateMirrorTextureDX(mOculusSystem.getSession(), ((GHRenderDeviceDX11*)mRenderServices.getDevice())->getD3DDevice().Get(), &mirrorTextureDesc, &mMirrorTexture);
	if (!GHOculusUtil::checkOvrResult(result, "ovr_CreateMirrorTextureDX"))
	{
		return;
	}
	
	// handle getting the mirror texture in to ghtexture.
	ID3D11Texture2D* dxtex = nullptr;
	ovr_GetMirrorTextureBufferDX(mOculusSystem.getSession(), mMirrorTexture, IID_PPV_ARGS(&dxtex));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView = nullptr;
	((GHRenderDeviceDX11*)mRenderServices.getDevice())->getD3DDevice()->CreateShaderResourceView(dxtex, &shaderResourceViewDesc, shaderResourceView.GetAddressOf());

	mMirrorGHTexture = new GHTextureDX11(*(GHRenderDeviceDX11*)(mRenderServices.getDevice()), dxtex, shaderResourceView, nullptr, "", 0, 0, 0, 0, 0, true, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	mMirrorGHTexture->acquire();

	// add a callback to get the texture out to shaders.
	GHMaterialCallbackFactory* hmdMirrorTextureCallback = new GHMaterialCallbackFactoryTexture("HMDMirrorTexture", GHMaterialCallbackType::CT_PERFRAME, mMirrorGHTexture);
	mRenderServices.getMaterialCallbackMgr()->addFactory(hmdMirrorTextureCallback);
}

void GHOculusDX11HMDRenderDevice::initRenderTarget(void)
{
	float backbufferMult = 2.0;
	if (mGraphicsQuality < 2)
	{
		backbufferMult = 1.0;
	}
	ovrSizei leftSize = ovr_GetFovTextureSize(mOculusSystem.getSession(), ovrEye_Left, mOculusSystem.getHMDDesc().DefaultEyeFov[ovrEye_Left], backbufferMult);
	ovrSizei rightSize = ovr_GetFovTextureSize(mOculusSystem.getSession(), ovrEye_Right, mOculusSystem.getHMDDesc().DefaultEyeFov[ovrEye_Right], backbufferMult);
	mRenderTarget = new GHOculusDX11RenderTarget(mOculusSystem.getSession(), *(mRenderServices.getDeviceDX11()), leftSize, rightSize);
}

void GHOculusDX11HMDRenderDevice::reinit(void)
{
	if (mRenderTarget)
	{
		delete mRenderTarget;
		mRenderTarget = 0;
	}
	initRenderTarget();
}

GHOculusDX11HMDRenderDevice::MessageHandler::MessageHandler(GHOculusDX11HMDRenderDevice& parent, GHEventMgr& eventMgr)
	: mEventMgr(eventMgr)
	, mParent(parent)
{
	mEventMgr.registerListener(GHRenderProperties::DEVICEREINIT, *this);
}

GHOculusDX11HMDRenderDevice::MessageHandler::~MessageHandler(void)
{
	mEventMgr.unregisterListener(GHRenderProperties::DEVICEREINIT, *this);
}

void GHOculusDX11HMDRenderDevice::MessageHandler::handleMessage(const GHMessage& message)
{
	mParent.reinit();
}
