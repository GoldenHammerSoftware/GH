#ifndef GH_DX12

#include "GHOculusDX11HMDServices.h"
#include "GHRenderServicesDX11.h"
#include "GHSystemServices.h"
#include "GHOculusDX11HMDRenderDevice.h"
#include "GHRenderDevice.h"

GHOculusDX11HMDServices::GHOculusDX11HMDServices(GHRenderServicesDX11& renderServices, 
	GHSystemServices& systemServices, GHControllerMgr& controllerMgr)
	: GHOculusHMDServices(renderServices, systemServices, controllerMgr)
{
	assert(renderServices.getDevice());
	mHMDRenderDevice = new GHOculusDX11HMDRenderDevice(renderServices, mOculusSystem, systemServices.getEventMgr());
	mRenderServices.getDevice()->setHMDDevice(mHMDRenderDevice);
}

GHOculusDX11HMDServices::~GHOculusDX11HMDServices(void)
{
}

#endif
