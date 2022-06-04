#ifndef GH_DX12

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
	: GHOculusHMDRenderDevice(renderServices, oculusSystem, eventMgr, renderServices.getDeviceDX11()->getGraphicsQuality())
{
	createMirrorTexture();
	initRenderTarget();
}

GHOculusDX11HMDRenderDevice::~GHOculusDX11HMDRenderDevice(void)
{
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
	mRenderTarget = new GHOculusDX11RenderTarget(mOculusSystem.getSession(), *(((GHRenderServicesDX11&)mRenderServices).getDeviceDX11()), leftSize, rightSize);
}

#endif