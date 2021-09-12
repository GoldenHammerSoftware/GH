#include "GHOpenVRSystem.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderProperties.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHUtilsIdentifiers.h"

GHOpenVRSystem::GHOpenVRSystem(GHMessageHandler& messageHandler)
	: mMessageHandler(messageHandler)
{
	init();
}

GHOpenVRSystem::~GHOpenVRSystem(void)
{
	shutdown();
}

void GHOpenVRSystem::init(void)
{
	vr::EVRInitError error = vr::VRInitError_None;
	mVRSystem = vr::VR_Init(&error, vr::VRApplication_Scene);
	if (error != vr::VRInitError_None)
	{
		GHDebugMessage::outputString("OpenVR Api VR_INIT failed: %s", vr::VR_GetVRInitErrorAsEnglishDescription(error));
		return;
	}

	if (!vr::VRCompositor())
	{
		GHDebugMessage::outputString("OpenVR Api failed to init VRCompositor");
		return;
	}

	mVRSystem->GetRecommendedRenderTargetSize(&mWidth, &mHeight);
}

void GHOpenVRSystem::shutdown(void)
{
	vr::VR_Shutdown();

	mVRSystem = 0;
	mWidth = 0;
	mHeight = 0;
}

void GHOpenVRSystem::updateHmdActive(void)
{
	vr::EDeviceActivityLevel activityLevel = mVRSystem->GetTrackedDeviceActivityLevel(vr::k_unTrackedDeviceIndex_Hmd);
	if (activityLevel == vr::k_EDeviceActivityLevel_UserInteraction)
	{
		if (!mHMDIsActive)
		{
			GHMessage setMsg(GHMessageTypes::SETAPPPROPERTY);
			setMsg.getPayload().setProperty(GHUtilsIdentifiers::PROPID, &GHRenderProperties::GP_HMDACTIVATED);
			setMsg.getPayload().setProperty(GHUtilsIdentifiers::PROPVAL, true);
			mMessageHandler.handleMessage(setMsg);

			mMessageHandler.handleMessage(GHMessage(GHRenderProperties::M_HMDACTIVATED));
			mHMDIsActive = true;
		}
	}
	else
	{
		if (mHMDIsActive)
		{
			GHMessage setMsg(GHMessageTypes::SETAPPPROPERTY);
			setMsg.getPayload().setProperty(GHUtilsIdentifiers::PROPID, &GHRenderProperties::GP_HMDACTIVATED);
			setMsg.getPayload().setProperty(GHUtilsIdentifiers::PROPVAL, false);
			mMessageHandler.handleMessage(setMsg);

			mMessageHandler.handleMessage(GHMessage(GHRenderProperties::M_HMDDEACTIVATED));
			mHMDIsActive = false;
		}
	}
}

void GHOpenVRSystem::update(void) 
{
	updateHmdActive();

	//vr::VREvent_t event;
	//while (mVRSystem->PollNextEvent(&event, sizeof(event)))
	//{
	//	switch (event.eventType)
	//	{
	//	case vr::VREvent_TrackedDeviceUserInteractionStarted:
	//		//GHDebugMessage::outputString("VREvent_TrackedDeviceUserInteractionStarted");
	//		break;
	//	case vr::VREvent_TrackedDeviceUserInteractionEnded:
	//		//GHDebugMessage::outputString("VREvent_TrackedDeviceUserInteractionEnded");
	//		break;
	//	case vr::VREvent_EnterStandbyMode:
	//		//mMessageHandler.handleMessage(GHMessage(GHRenderProperties::M_HMDDEACTIVATED));
	//		break;
	//	case vr::VREvent_LeaveStandbyMode:
	//		//mMessageHandler.handleMessage(GHMessage(GHRenderProperties::M_HMDACTIVATED));
	//		break;
	//
	//	case vr::VREvent_InputFocusChanged:
	//		//mMessageHandler.handleMessage(GHMessage(GHRenderProperties::M_HMDACTIVATED));
	//	{
	//		int breakpoint = 6;
	//		breakpoint = 7;
	//	}
	//		break;
	//	
	//
	//	//events we get but aren't doing anything with
	//	case vr::VREvent_SceneApplicationStateChanged:
	//	case vr::VREvent_ProcessConnected:
	//	case vr::VREvent_Input_ActionManifestReloaded:
	//	case vr::VREvent_Input_BindingLoadSuccessful:
	//	case vr::VREvent_ActionBindingReloaded:
	//	case vr::VREvent_Input_BindingsUpdated:
	//	case vr::VREvent_Compositor_ApplicationResumed:
	//	case vr::VREvent_SceneApplicationChanged:
	//	case vr::VREvent_Compositor_ApplicationNotResponding: //Probably getting this one because of sitting on breakpoint
	//	case vr::VREvent_StatusUpdate:
	//	case vr::VREvent_InputFocusCaptured: //deprecated
	//	case vr::VREvent_InputFocusReleased: //deprecated
	//	//case vr::VREvent_InputFocusChanged: //we may want to use this
	//	case vr::VREvent_PropertyChanged: //we may want to use this
	//	case vr::VREvent_ButtonPress: //for some reason we're getting this when we touch the headset?
	//	case vr::VREvent_ButtonUnpress: //for some reason we're getting this when we touch the headset?
	//		break;
	//
	//	default:
	//	{
	//		int breakpoint = 6;
	//		breakpoint = 7;
	//	}
	//		break;
	//	}
	//}
}
