#pragma once

#include "GHHMDServices.h"
#include "GHOculusSystemController.h"

class GHOculusHMDRenderDevice;
class GHRenderServices;
class GHSystemServices;
class GHControllerMgr;
class GHOculusTouchInputTranslator;
class GHOculusAvatarPoserFactory;

// place to construct oculus specific stuff for use elsewhere.
class GHOculusHMDServices : public GHHMDServices
{
public:
	GHOculusHMDServices(GHRenderServices& renderServices, GHSystemServices& systemServices,
		GHControllerMgr& controllerMgr);
	~GHOculusHMDServices(void);

	virtual void initAppShard(GHAppShard& appShard, const GHTransform& hmdOrigin, const GHTimeVal& realTimeVal) override;
	virtual GHController* createHMDTransformApplier(GHTransform& outTrans) override;

protected:
	GHRenderServices& mRenderServices;
	GHSystemServices& mSystemServices;
	GHControllerMgr& mControllerMgr;

	GHOculusSystemController mOculusSystem;
	GHOculusAvatarPoserFactory* mAvatarPoserFactory{ 0 };
	GHOculusHMDRenderDevice* mHMDRenderDevice{ 0 };
	GHOculusTouchInputTranslator* mTouchInputTranslator{ 0 };
	const GHTransform* mHMDOrigin{ 0 }; // the game-specific origin point for offsetting the oculus position.
};
