#pragma once
#ifndef GH_DX12

#include "GHOculusHMDServices.h"

class GHRenderServicesDX11;

// place to construct oculus specific stuff for use elsewhere.
class GHOculusDX11HMDServices : public GHOculusHMDServices
{
public:
	GHOculusDX11HMDServices(GHRenderServicesDX11& renderServices, GHSystemServices& systemServices, 
		GHControllerMgr& controllerMgr);
	~GHOculusDX11HMDServices(void);
};

#endif
