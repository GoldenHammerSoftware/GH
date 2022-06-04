#pragma once

#ifdef GH_DX12

#include "GHOculusHMDRenderDevice.h"

class GHRenderServicesDX12;

class GHOculusDX12HMDRenderDevice : public GHOculusHMDRenderDevice
{
public:
	GHOculusDX12HMDRenderDevice(GHRenderServicesDX12& renderServices, const GHOculusSystemController& oculusSystem, GHEventMgr& eventMgr);
	~GHOculusDX12HMDRenderDevice(void);

protected:
	virtual void createMirrorTexture(void) override;
	virtual void initRenderTarget(void) override;
};

#endif
