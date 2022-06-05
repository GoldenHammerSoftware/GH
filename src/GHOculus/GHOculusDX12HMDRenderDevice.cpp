#ifdef GH_DX12
#include "GHOculusDX12HMDRenderDevice.h"
#include "GHDX12/GHRenderServicesDX12.h"
#include "GHOculusSystemController.h"
#include "GHOculusDX12RenderTarget.h"

GHOculusDX12HMDRenderDevice::GHOculusDX12HMDRenderDevice(GHRenderServicesDX12& renderServices,
	const GHOculusSystemController& oculusSystem,
	GHEventMgr& eventMgr)
	: GHOculusHMDRenderDevice(renderServices, oculusSystem, eventMgr, renderServices.getGraphicsQuality())
{
	createMirrorTexture();
	initRenderTarget();
}

GHOculusDX12HMDRenderDevice::~GHOculusDX12HMDRenderDevice(void)
{
}

void GHOculusDX12HMDRenderDevice::createMirrorTexture(void)
{
}

void GHOculusDX12HMDRenderDevice::initRenderTarget(void)
{
	float backbufferMult = 2.0;
	if (mGraphicsQuality < 2)
	{
		backbufferMult = 1.0;
	}
	ovrSizei leftSize = ovr_GetFovTextureSize(mOculusSystem.getSession(), ovrEye_Left, mOculusSystem.getHMDDesc().DefaultEyeFov[ovrEye_Left], backbufferMult);
	ovrSizei rightSize = ovr_GetFovTextureSize(mOculusSystem.getSession(), ovrEye_Right, mOculusSystem.getHMDDesc().DefaultEyeFov[ovrEye_Right], backbufferMult);
	GHRenderDeviceDX12* dx12Device = (GHRenderDeviceDX12*)mRenderServices.getDevice();
	mRenderTarget = new GHOculusDX12RenderTarget(mOculusSystem.getSession(), *dx12Device, leftSize, rightSize);
}

#endif

