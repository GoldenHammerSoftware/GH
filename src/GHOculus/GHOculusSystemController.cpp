#include "GHOculusSystemController.h"
#include "GHPlatform/GHDebugMessage.h"
#include <algorithm> //for std::max
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h" //for M_HMDACTIVATED/M_HMDDEACTIVATED
#include "GHMessageTypes.h"
#include "GHUtils/GHUtilsIdentifiers.h"
#include "GHOculusGameScale.h"

#include "GHDebugDraw.h"
#include "GHMath/GHQuaternion.h"
#include "GHMath/GHTransform.h"
#include "GHOculusUtil.h"
#include <OVR_CAPI_Audio.h>
#include "GHBaseIdentifiers.h" //for M_NEWAUDIODEVICEAVAILABLE/MP_DESIREDAUDIODEVICEGUIDSTR

GHOculusSystemController::GHOculusSystemController(GHEventMgr& eventMgr)
	: mEventMgr(eventMgr)
	, mInputStates { InputState(ovrControllerType_XBox, InputState::GI_CONTROLLER),
					 InputState(ovrControllerType_Touch, InputState::GI_TOUCH),
					 InputState(ovrControllerType_Remote, InputState::GI_REMOTE) }
{
	ovrInitParams initParams = { ovrInit_RequestVersion, OVR_MINOR_VERSION, NULL, 0, 0 };
	ovrResult result = ovr_Initialize(&initParams);
	if (!GHOculusUtil::checkOvrResult(result, "ovr_Initialize"))
	{
		return;
	}

	ovrGraphicsLuid luid;
	result = ovr_Create(&mSession, &luid);
	if (!GHOculusUtil::checkOvrResult(result, "ovr_Create"))
	{
		return;
	}

	mHMDDesc = ovr_GetHmdDesc(mSession);

	unsigned int trackerCount = std::max<unsigned int>(1, ovr_GetTrackerCount(mSession)); //from example code: Make sure there's always at least one
	mTrackerDescs.reserve(trackerCount);
	for (unsigned int i = 0; i < trackerCount; ++i)
	{
		mTrackerDescs.push_back(ovr_GetTrackerDesc(mSession, i));
	}

	result = ovr_SetTrackingOriginType(mSession, ovrTrackingOrigin_FloorLevel);
	GHOculusUtil::checkOvrResult(result, "ovr_SetTrackingOriginType");

	// make sure our tracking state is valid.
	double ftiming = ovr_GetPredictedDisplayTime(mSession, 0);
	mTrackingState = ovr_GetTrackingState(mSession, ftiming, ovrTrue);

	//set audio device
	{
		WCHAR deviceGuidStr[OVR_AUDIO_MAX_DEVICE_STR_SIZE];
		ovr_GetAudioDeviceOutGuidStr(deviceGuidStr);

		GHPropertyContainer messageProps;
		messageProps.setProperty(GHBaseIdentifiers::MP_DESIREDAUDIODEVICEGUIDSTR, deviceGuidStr);
		mEventMgr.handleMessage(GHMessage(GHBaseIdentifiers::M_NEWAUDIODEVICEAVAILABLE, &messageProps));
	}
}

GHOculusSystemController::~GHOculusSystemController(void)
{
	//potential order-of-operation issue with shutting down DirectX and OVR
	ovr_Shutdown();
}

void GHOculusSystemController::onActivate(void)
{

}

void GHOculusSystemController::onDeactivate(void)
{

}

void GHOculusSystemController::update(void)
{
	ovrSessionStatus sessionStatus;
	ovr_GetSessionStatus(mSession, &sessionStatus);
	if (sessionStatus.ShouldQuit)
	{
		//no, you are trapped in VR forever. muahahaha
	}

	if (sessionStatus.ShouldRecenter)
	{
		ovr_RecenterTrackingOrigin(mSession);
	}

	//head tracking
	mEyeRenderDesc[ovrEye_Left] = ovr_GetRenderDesc(mSession, ovrEye_Left, mHMDDesc.DefaultEyeFov[ovrEye_Left]);
	mEyeRenderDesc[ovrEye_Right] = ovr_GetRenderDesc(mSession, ovrEye_Right, mHMDDesc.DefaultEyeFov[ovrEye_Right]);

	ovrPosef useHmdToEyeOffset[2] = { mEyeRenderDesc[0].HmdToEyePose, mEyeRenderDesc[1].HmdToEyePose };

	double ftiming = ovr_GetPredictedDisplayTime(mSession, 0);
	mTrackingState = ovr_GetTrackingState(mSession, ftiming, ovrTrue);

	ovr_CalcEyePoses(mTrackingState.HeadPose.ThePose, useHmdToEyeOffset, mEyeRenderPose);

	//end head tracking
	bool isVisible = (sessionStatus.IsVisible == ovrTrue);
	isVisible &= (sessionStatus.HmdMounted == ovrTrue);
	if (isVisible != mIsVisible)
	{
		mIsVisible = isVisible;
		if (mIsVisible) {
			GHMessage setMsg(GHMessageTypes::SETAPPPROPERTY);
			setMsg.getPayload().setProperty(GHUtilsIdentifiers::PROPID, &GHRenderProperties::GP_HMDACTIVATED);
			setMsg.getPayload().setProperty(GHUtilsIdentifiers::PROPVAL, true);
			mEventMgr.handleMessage(setMsg);

			GHMessage activatedMessage(GHRenderProperties::M_HMDACTIVATED);
			mEventMgr.handleMessage(activatedMessage);
		}
		else {
			GHMessage setMsg(GHMessageTypes::SETAPPPROPERTY);
			setMsg.getPayload().setProperty(GHUtilsIdentifiers::PROPID, &GHRenderProperties::GP_HMDACTIVATED);
			setMsg.getPayload().setProperty(GHUtilsIdentifiers::PROPVAL, false);
			mEventMgr.handleMessage(setMsg);

			GHMessage deactivatedMessage(GHRenderProperties::M_HMDDEACTIVATED);
			mEventMgr.handleMessage(deactivatedMessage);
		}
	}

	//input poll
	for (int i = 0; i < InputState::GI_NUMINPUTSTATES; ++i)
	{
		InputState& is = mInputStates[i];
		is.mIsActive = ovr_GetInputState(mSession, is.mControllerType, &is.mOvrInput) == ovrSuccess;
	}
}

const GHOculusSystemController::InputState& GHOculusSystemController::getInputState(InputState::GamepadIndex gamepadIndex) const
{
	assert(gamepadIndex < InputState::GI_NUMINPUTSTATES);
	return mInputStates[gamepadIndex];
}

float GHOculusSystemController::getOculusToGameScale(void) const
{
	return GHOculusUtil::getOculusToGameScale();
}

float GHOculusSystemController::getGameToOculusScale(void) const
{
	return GHOculusUtil::getGameToOculusScale();
}