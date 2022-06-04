#ifdef GH_DX12
#include "GHOculusDX12HMDServices.h"
#include "GHDX12/GHRenderServicesDX12.h"
#include "GHOculusDX12HMDRenderDevice.h"
#include "GHSystemServices.h"
#include "Render/GHRenderDevice.h"

GHOculusDX12HMDServices::GHOculusDX12HMDServices(GHRenderServicesDX12& renderServices,
	GHSystemServices& systemServices, GHControllerMgr& controllerMgr)
	: GHOculusHMDServices(renderServices, systemServices, controllerMgr)
{
	assert(renderServices.getDevice());
	mHMDRenderDevice = new GHOculusDX12HMDRenderDevice(renderServices, mOculusSystem, systemServices.getEventMgr());
	mRenderServices.getDevice()->setHMDDevice(mHMDRenderDevice);
}

GHOculusDX12HMDServices::~GHOculusDX12HMDServices(void)
{
}

#endif

