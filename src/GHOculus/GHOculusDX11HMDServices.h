#pragma once

#include "GHHMDServices.h"
#include "GHOculusSystemController.h"

class GHOculusDX11HMDRenderDevice;
class GHRenderServicesDX11;
class GHSystemServices;
class GHControllerMgr;
class GHOculusTouchInputTranslator;
class GHOculusAvatarPoserFactory;

// place to construct oculus specific stuff for use elsewhere.
class GHOculusDX11HMDServices : public GHHMDServices
{
public:
	GHOculusDX11HMDServices(GHRenderServicesDX11& renderServices, GHSystemServices& systemServices, 
		GHControllerMgr& controllerMgr);
	~GHOculusDX11HMDServices(void);

	virtual void initAppShard(GHAppShard& appShard, const GHTransform& hmdOrigin, const GHTimeVal& realTimeVal);
	virtual GHController* createHMDTransformApplier(GHTransform& outTrans);

private:
	GHRenderServicesDX11& mRenderServices;
	GHSystemServices& mSystemServices;
	GHControllerMgr& mControllerMgr;

	GHOculusSystemController mOculusSystem;
	GHOculusAvatarPoserFactory* mAvatarPoserFactory{ 0 };
	GHOculusDX11HMDRenderDevice* mHMDRenderDevice{ 0 };
	GHOculusTouchInputTranslator* mTouchInputTranslator{ 0 };
	const GHTransform* mHMDOrigin{ 0 }; // the game-specific origin point for offsetting the oculus position.
};

