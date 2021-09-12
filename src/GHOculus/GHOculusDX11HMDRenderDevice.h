#pragma once

#include "OVR_CAPI_D3D.h"

#include "GHHMDRenderDevice.h"
#include <vector>
#include "GHUtils/GHController.h"
#include "GHUtils/GHMessageHandler.h"

class GHRenderDeviceDX11;
class GHViewInfo;
class GHCamera;
class GHOculusDX11RenderTarget;
class GHControllerMgr;
class GHMessageHandler;
class GHRenderServicesDX11;
class GHTextureDX11;
class GHOculusSystemController;
class GHEventMgr;

class GHOculusDX11HMDRenderDevice : public GHHMDRenderDevice
{
public:
	GHOculusDX11HMDRenderDevice(GHRenderServicesDX11& renderServices, const GHOculusSystemController& oculusSystem, GHEventMgr& eventMgr);
	~GHOculusDX11HMDRenderDevice(void);

	virtual void beginFrame(void);
	virtual void endFrame(void);

	virtual void createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
		GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const;

	virtual bool isActive(void) const;
	virtual GHRenderTarget* getRenderTarget(RenderTargetType targetType) const;

	const ovrTrackingState& getTrackingState(void);
	float getOculusToGameScale(void) const;

	void reinit(void);

private:
	void createMirrorTexture(void);
	void initRenderTarget(void);

private:
	class MessageHandler : public GHMessageHandler
	{
	public:
		MessageHandler(GHOculusDX11HMDRenderDevice& parent, GHEventMgr& eventMgr);
		~MessageHandler(void);

		virtual void handleMessage(const GHMessage& message);

	private:
		GHOculusDX11HMDRenderDevice& mParent;
		GHEventMgr& mEventMgr;
	};

private:
	const GHOculusSystemController& mOculusSystem;

	//HINSTANCE	mHInst;
	GHRenderServicesDX11&		mRenderServices;
	GHOculusDX11RenderTarget*	mRenderTarget { nullptr };
	
	ovrMirrorTexture			mMirrorTexture { nullptr }; //for seeing output on pc
	GHTextureDX11*				mMirrorGHTexture { nullptr };
	long long					mFrameIndex { 0 };

	float						mAspectRatio { 1.0 };

	MessageHandler				mMessageHandler;
	const int&					mGraphicsQuality;
};
