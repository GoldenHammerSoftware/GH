#pragma once

#include "GHOculusHMDRenderDevice.h"

class GHRenderServicesDX11;

class GHOculusDX11HMDRenderDevice : public GHOculusHMDRenderDevice
{
public:
	GHOculusDX11HMDRenderDevice(GHRenderServicesDX11& renderServices, const GHOculusSystemController& oculusSystem, GHEventMgr& eventMgr);
	~GHOculusDX11HMDRenderDevice(void);

protected:
	virtual void createMirrorTexture(void) override;
	virtual void initRenderTarget(void) override;
};
