#ifdef GH_DX12
#include "GHOculusDX12HMDRenderDevice.h"
#include "GHDX12/GHRenderServicesDX12.h"

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
}

#endif

