#pragma once

#ifdef GH_OVR_GO

#include <jni.h>
#include "GHHMDServices.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHOvrGoSystemController.h"
#include "GHOvrGoFrameState.h"

#include "VrApi.h"
#include "VrApi_Helpers.h"
#include "VrApi_SystemUtils.h"

class GHGLESRenderServices;
class GHSystemServices;
class GHOvrGoHMDRenderDevice;
class GHJNIMgr;
class GHEventMgr;
class GHControllerMgr;
class GHOvrGoAvatarPoserFactory;
class GHInputState;

class GHOvrGoHMDServices : public GHHMDServices
{
public:
	GHOvrGoHMDServices(GHGLESRenderServices& renderServices, GHSystemServices& systemServices, GHControllerMgr& controllerMgr,
		GHInputState& platformInputState, GHJNIMgr& jniMgr, jobject activity);
	~GHOvrGoHMDServices(void);

	virtual void initAppShard(GHAppShard& appShard, const GHTransform& hmdOrigin, const GHTimeVal& realTimeVal);
	virtual GHController* createHMDTransformApplier(GHTransform& outTrans);

	void enterVRMode(void);
	void exitVRMode(void);

private:
	void initOvr(void);
	// might need to re-init ovr if the jni args changed.
	void verifyJNI(void);

private:
	class MessageHandler : public GHMessageHandler
	{
	public:
		MessageHandler(GHOvrGoHMDServices& parent, GHEventMgr& eventMgr);
		~MessageHandler(void);

		virtual void handleMessage(const GHMessage& message);

	private:
		GHOvrGoHMDServices & mParent;
		GHEventMgr& mEventMgr;
	};

private:
	GHSystemServices& mSystemServices;
	GHGLESRenderServices& mRenderServices;
	GHJNIMgr& mJNIMgr;
	GHControllerMgr& mControllerMgr;
	GHInputState& mPlatformInputState;
	MessageHandler mMessageHandler;
	GHOvrGoHMDRenderDevice* mHMDRenderDevice;
	ovrJava mOvrJavaDesc;
	ovrMobile* mOvrMobile{ 0 };
	const GHTransform* mHMDOrigin{ 0 }; // the game-specific origin point for offsetting the oculus position.
	GHOvrGoFrameState mFrameState;
	GHOvrGoSystemController* mOvrController{ 0 };
	GHOvrGoAvatarPoserFactory* mAvatarPoserFactory{ 0 };
};

#endif