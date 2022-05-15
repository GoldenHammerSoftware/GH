#pragma once

#include "OVR_CAPI_D3D.h"

#include "GHHMDRenderDevice.h"
#include <vector>
#include "GHUtils/GHController.h"
#include "GHUtils/GHMessageHandler.h"

class GHViewInfo;
class GHCamera;
class GHControllerMgr;
class GHMessageHandler;
class GHOculusSystemController;
class GHEventMgr;
class GHRenderServices;
class GHOculusRenderTarget;
class GHTexture;

class GHOculusHMDRenderDevice : public GHHMDRenderDevice
{
public:
	GHOculusHMDRenderDevice(GHRenderServices& renderServices, const GHOculusSystemController& oculusSystem, GHEventMgr& eventMgr, const int& graphicsQuality);
	~GHOculusHMDRenderDevice(void);

	virtual void beginFrame(void);
	virtual void endFrame(void);

	virtual void createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
		GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const;

	virtual bool isActive(void) const;
	virtual GHRenderTarget* getRenderTarget(RenderTargetType targetType) const;

	const ovrTrackingState& getTrackingState(void);
	float getOculusToGameScale(void) const;

	void reinit(void);

protected:
	virtual void createMirrorTexture(void) = 0;
	virtual void initRenderTarget(void) = 0;

private:
	class MessageHandler : public GHMessageHandler
	{
	public:
		MessageHandler(GHOculusHMDRenderDevice& parent, GHEventMgr& eventMgr);
		~MessageHandler(void);

		virtual void handleMessage(const GHMessage& message);

	private:
		GHOculusHMDRenderDevice& mParent;
		GHEventMgr& mEventMgr;
	};

protected:
	const GHOculusSystemController& mOculusSystem;

	GHRenderServices& mRenderServices;
	GHOculusRenderTarget* mRenderTarget{ nullptr };

	ovrMirrorTexture mMirrorTexture{ nullptr }; //for seeing output on pc
	GHTexture* mMirrorGHTexture{ nullptr };
	long long mFrameIndex{ 0 };

	float mAspectRatio{ 1.0 };

	const int& mGraphicsQuality;

private:
	MessageHandler mMessageHandler;
};
