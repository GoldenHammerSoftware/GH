#pragma once

#include "GHHMDServices.h"
#include "GHOpenVRSystem.h"

class GHOpenVRDX11HMDRenderDevice;
class GHRenderServicesDX11;
class GHSystemServices;
class GHControllerMgr;
class GHOpenVRInputHandler;
class GHOculusAvatarPlatformPoserFactory;

class GHOpenVRDX11HMDServices : public GHHMDServices
{
public:
	GHOpenVRDX11HMDServices(GHRenderServicesDX11& renderServices, GHSystemServices& systemServices,
							GHControllerMgr& controllerMgr);
	~GHOpenVRDX11HMDServices(void);

	virtual void initAppShard(GHAppShard& appShard, const GHTransform& hmdOrigin, const GHTimeVal& realTimeVal);
	virtual GHController* createHMDTransformApplier(GHTransform& outTrans);

private:
	GHRenderServicesDX11& mRenderServices;
	GHSystemServices& mSystemServices;
	GHControllerMgr& mControllerMgr;

	GHOpenVRSystem mOpenVR;
	GHOculusAvatarPlatformPoserFactory* mAvatarPoserFactory{ 0 };
	GHOpenVRDX11HMDRenderDevice* mHMDRenderDevice{ 0 };
	GHOpenVRInputHandler* mInputHandler{ 0 };
	const GHTransform* mHMDOrigin{ 0 }; // the game-specific origin point for offsetting the hmd position.
};

