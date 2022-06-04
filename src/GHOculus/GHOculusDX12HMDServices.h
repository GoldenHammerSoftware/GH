#pragma once
#ifdef GH_DX12

#include "GHOculusHMDServices.h"

class GHRenderServicesDX12;

// place to construct oculus specific stuff for use elsewhere.
class GHOculusDX12HMDServices : public GHOculusHMDServices
{
public:
	GHOculusDX12HMDServices(GHRenderServicesDX12& renderServices, GHSystemServices& systemServices,
		GHControllerMgr& controllerMgr);
	~GHOculusDX12HMDServices(void);
};

#endif

