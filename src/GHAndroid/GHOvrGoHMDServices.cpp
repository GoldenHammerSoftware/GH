#ifdef GH_OVR_GO

#include "GHOvrGoHMDServices.h"
#include "GHGLESRenderServices.h"
#include "GHRenderDevice.h"
#include "GHSystemServices.h"
#include "GHOvrGoHMDRenderDevice.h"
#include "GHPlatform/android/GHJNIMgr.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHAndroidEGLInfo.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"
#include "GHOvrGoHMDTransformApplier.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHOvrGoAvatarPoser.h"
#include "GHAppShard.h"
#include "GHOculusAvatarXMLLoader.h"
#include "GHOvrGoTextureLoader.h"
#include "OVR_Platform.h"

GHOvrGoHMDServices::GHOvrGoHMDServices(GHGLESRenderServices& renderServices, GHSystemServices& systemServices,
	GHControllerMgr& controllerMgr, GHInputState& platformInputState, GHJNIMgr& jniMgr, jobject activity)
: mMessageHandler(*this, systemServices.getEventMgr())
, mJNIMgr(jniMgr)
, mControllerMgr(controllerMgr)
, mSystemServices(systemServices)
, mRenderServices(renderServices)
, mPlatformInputState(platformInputState)
{
	mOvrJavaDesc.Vm = mJNIMgr.getJVM();
	mOvrJavaDesc.Env = &mJNIMgr.getJNIEnv();
	mOvrJavaDesc.ActivityObject = activity;

	mHMDRenderDevice = new GHOvrGoHMDRenderDevice(renderServices, systemServices.getPlatformServices().getThreadFactory(), mOvrJavaDesc, mFrameState);
	renderServices.getDevice()->setHMDDevice(mHMDRenderDevice);

	initOvr();

	// needs to be called after mHMDRenderDevice is created.
	// correct behavior is to call this on android unpause.
	enterVRMode();
}

GHOvrGoHMDServices::~GHOvrGoHMDServices(void)
{
	mControllerMgr.remove(mOvrController);
	delete mAvatarPoserFactory;
	delete mOvrController;

	exitVRMode();
	vrapi_Shutdown();
}

void GHOvrGoHMDServices::initAppShard(GHAppShard& appShard, const GHTransform& hmdOrigin, const GHTimeVal& realTimeVal)
{
	mHMDOrigin = &hmdOrigin;

	// todo: inject this
	#define GHBowlingGOAppId "1824917740900089"
	const char* appID = GHBowlingGOAppId;

	mOvrController = new GHOvrGoSystemController(mFrameState, mPlatformInputState, *mHMDOrigin);
	mControllerMgr.add(mOvrController);

	mAvatarPoserFactory = new GHOvrGoAvatarPoserFactory(mFrameState, mOvrController->getOvrInputState());

	GHOvrGoTextureLoader* textureLoader = new GHOvrGoTextureLoader(mRenderServices.getTextureMgr(), mRenderServices.getStateCache());
	appShard.mResourceFactory.addLoader(textureLoader, 1, ".ovrtex");

	//platform sdk stuff. 
	ovrPlatformInitializeResult result = ovr_PlatformInitializeAndroid(appID, mOvrJavaDesc.ActivityObject, mOvrJavaDesc.Env);

	ovrID userID = ovr_GetLoggedInUserID();

	if (result == ovrPlatformInitialize_Success)
	{
        //initialize the avatar module
        ovrAvatar_InitializeAndroid(appID, mOvrJavaDesc.ActivityObject, mOvrJavaDesc.Env);

        appShard.mXMLObjFactory.addLoader(new GHOculusAvatarXMLLoader(GHBowlingGOAppId, userID, appShard.mXMLObjFactory, *mHMDOrigin, *mAvatarPoserFactory,
        									  *mRenderServices.getVBFactory(), *mRenderServices.getMaterialCallbackMgr(), mSystemServices.getPlatformServices().getIdFactory(),
        									  mSystemServices.getInputState(), appShard.mProps, mSystemServices.getXMLSerializer(), mSystemServices.getEventMgr(),
        									  appShard.mResourceFactory),
        									  1, "hmdAvatarController");
	}
	else
	{
	    GHDebugMessage::outputString("ovr_PlatformInitializeAndroid failed with result %d", result);
	}
}

GHController* GHOvrGoHMDServices::createHMDTransformApplier(GHTransform& outTrans)
{
	GHOvrGoHMDTransformApplier* ret = new GHOvrGoHMDTransformApplier(mFrameState, *mHMDOrigin, outTrans);
	return ret;
}

void GHOvrGoHMDServices::initOvr(void)
{
	// Initialize VrApi.
	const ovrInitParms initParms = vrapi_DefaultInitParms(&mOvrJavaDesc);
	ovrInitializeStatus vrapiInitResult = vrapi_Initialize(&initParms);
	if (vrapiInitResult != VRAPI_INITIALIZE_SUCCESS)
	{
		GHDebugMessage::outputString("Failed to initialize VrApi!");

		if (VRAPI_INITIALIZE_UNKNOWN_ERROR == vrapiInitResult) {
			GHDebugMessage::outputString("Oculus is probably not present on this device");
		}

		if (VRAPI_INITIALIZE_PERMISSIONS_ERROR == vrapiInitResult) {
			GHDebugMessage::outputString("Thread priority security exception. Make sure the APK is signed.");
		}

		abort();
	}
}

void GHOvrGoHMDServices::verifyJNI(void)
{
	JNIEnv* newEnv = &mJNIMgr.getJNIEnv();
	if (newEnv == mOvrJavaDesc.Env)
	{
		// no changes, we're good.
		return;
	}
	GHDebugMessage::outputString("re-initing vrapi for new thread.");
	vrapi_Shutdown();

	mOvrJavaDesc.Vm = mJNIMgr.getJVM();
	mOvrJavaDesc.Env = &mJNIMgr.getJNIEnv();

	initOvr();
}

void GHOvrGoHMDServices::enterVRMode(void)
{
	if (mOvrMobile) {
		GHDebugMessage::outputString("enterVRMode called twice with no exitVRMode?");
		return;
	}
	verifyJNI();

	ovrModeParms modeParms = vrapi_DefaultModeParms(&mOvrJavaDesc);
	modeParms.Flags |= VRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN;
	modeParms.Flags |= VRAPI_MODE_FLAG_NATIVE_WINDOW;
	modeParms.Display = (size_t)GHAndroidEGLInfo::get()->getDisplay();
	modeParms.WindowSurface = (size_t)GHAndroidEGLInfo::get()->getWindow();
	modeParms.ShareContext = (size_t)GHAndroidEGLInfo::get()->getContext();

	mOvrMobile = vrapi_EnterVrMode(&modeParms);
	if (mOvrMobile == NULL)
	{
		GHDebugMessage::outputString("Failed to enter vr mode");
		return;
	}
	else
	{
		GHDebugMessage::outputString("Entered VR mode");
	}
	// 2(cpu), 2(gpu) is default.
	vrapi_SetClockLevels(mOvrMobile, 4, 4 );
	//vrapi_SetPerfThread( app->Ovr, VRAPI_PERF_THREAD_TYPE_MAIN, app->MainThreadTid );
	//vrapi_SetPerfThread( app->Ovr, VRAPI_PERF_THREAD_TYPE_RENDERER, app->RenderThreadTid );

	//vrapi_SetDisplayRefreshRate(mOvrMobile, 72.0f);

	int foveation_available = vrapi_GetSystemPropertyInt(&mOvrJavaDesc, VRAPI_SYS_PROP_FOVEATION_AVAILABLE);
	if (foveation_available)
	{
		GHDebugMessage::outputString("Enabling foveated rendering");
		vrapi_SetPropertyInt(&mOvrJavaDesc, VRAPI_FOVEATION_LEVEL, 3);
	}

	mHMDRenderDevice->setOvr(mOvrMobile);
	mFrameState.mOvrMobile = mOvrMobile;
}

void GHOvrGoHMDServices::exitVRMode(void)
{
	// todo: call this when the activity is paused, the Android Surface is
	// destroyed, or when switching to another activity.

	if (!mOvrMobile) {
		GHDebugMessage::outputString("exitVRMode called while not in vr mode");
		return;
	}

	vrapi_LeaveVrMode(mOvrMobile);
	mOvrMobile = NULL;
	mHMDRenderDevice->setOvr(0);
	mFrameState.mOvrMobile = 0;
	GHDebugMessage::outputString("Left VR mode");
}

GHOvrGoHMDServices::MessageHandler::MessageHandler(GHOvrGoHMDServices& parent, GHEventMgr& eventMgr)
	: mParent(parent)
	, mEventMgr(eventMgr)
{
	mEventMgr.registerListener(GHMessageTypes::PAUSEINTERRUPT, *this);
	mEventMgr.registerListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
}

GHOvrGoHMDServices::MessageHandler::~MessageHandler(void)
{
	mEventMgr.unregisterListener(GHMessageTypes::PAUSEINTERRUPT, *this);
	mEventMgr.unregisterListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
}

void GHOvrGoHMDServices::MessageHandler::handleMessage(const GHMessage& message)
{
	if (message.getType() == GHMessageTypes::UNPAUSEINTERRUPT)
	{
		mParent.enterVRMode();
	}
	else //GHMessageTypes::PAUSEINTERRUPT
	{
		mParent.exitVRMode();
	}
}

#endif