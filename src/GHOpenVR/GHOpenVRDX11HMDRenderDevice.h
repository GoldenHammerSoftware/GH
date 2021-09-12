#pragma once

#include "GHHMDRenderDevice.h"
#include <vector>
#include "GHUtils/GHController.h"
#include "GHUtils/GHMessageHandler.h"

class GHRenderDeviceDX11;
class GHViewInfo;
class GHCamera;
class GHControllerMgr;
class GHMessageHandler;
class GHRenderServicesDX11;
class GHTextureDX11;
class GHOpenVRSystem;
class GHEventMgr;
class GHOpenVREyeRenderTarget;

class GHOpenVRDX11HMDRenderDevice : public GHHMDRenderDevice
{
public:
	GHOpenVRDX11HMDRenderDevice(GHRenderServicesDX11& renderServices,
								GHOpenVRSystem& vrSystem, 
								GHEventMgr& eventMgr);
	~GHOpenVRDX11HMDRenderDevice(void);

	virtual void beginFrame(void);
	virtual void endFrame(void);

	virtual void createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
								  GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const;

	virtual bool isActive(void) const;
	virtual GHRenderTarget* getRenderTarget(RenderTargetType targetType) const;

private:
	void initRenderTarget(void);

private:
	GHOpenVRSystem&				mVRSystem;

	GHRenderServicesDX11&		mRenderServices;
	GHEventMgr& mEventMgr;
	GHOpenVREyeRenderTarget*	mLeftRenderTarget{ nullptr };
	GHOpenVREyeRenderTarget*	mRightRenderTarget{ nullptr };
	
	const int&					mGraphicsQuality;
};
