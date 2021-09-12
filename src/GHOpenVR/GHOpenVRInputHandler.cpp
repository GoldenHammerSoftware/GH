#include "GHOpenVRInputHandler.h"
#include "GHOpenVRSystem.h"
#include "GHInputState.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOpenVRUtil.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHMath/GHFloat.h"
#include "GHMath/GHTransform.h"
#include "GHMath/GHPi.h"

const int kGamepadIdx = 1;

GHOpenVRInputHandler::GHOpenVRInputHandler(const GHTransform& hmdOrigin, GHOpenVRSystem& system, GHInputState& inputState, const GHTimeVal& timeVal)
	: mHMDOrigin(hmdOrigin)
	, mVRSystem(system)
	, mInputState(inputState)
	, mTimeVal(timeVal)
{
#ifdef GH_OPENVR_DEBUG_HANDS
	mDebugCross[0].setSizes(0.1f, 0.1f, 0.1f);
	mDebugCross[1].setSizes(0.1f, 0.1f, 0.1f);
#endif
}

void GHOpenVRInputHandler::onActivate(void)
{

}

void GHOpenVRInputHandler::onDeactivate(void)
{

}

void GHOpenVRInputHandler::update(void)
{
	// WaitGetPoses should be called once per frame.
	vr::EVRCompositorError error = vr::VRCompositor()->WaitGetPoses(mTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
	if (error != vr::EVRCompositorError::VRCompositorError_None) {
		GHDebugMessage::outputString("VRCompositor::WaitGetPoses failed with error %u", error);
	}

	updatePoseables();
	mActionManager.updateActions(mInputState);
	updateImpliedActions();
}

void GHOpenVRInputHandler::updatePose(const vr::TrackedDevicePose_t& inPose, GHInputStructs::Poseable& outPose) const
{
	if (!inPose.bDeviceIsConnected || !inPose.bPoseIsValid)
	{
		return;
	}

	GHPoint3 originPos;
	mHMDOrigin.getTranslate(originPos);

	GHTransform originRot = mHMDOrigin;
	originRot.setTranslate(GHPoint3(0, 0, 0));

	if (inPose.eTrackingResult == vr::TrackingResult_Running_OK)
	{
		GHOpenVRUtil::convertToGHTransform(inPose.mDeviceToAbsoluteTracking, outPose.mPosition);

		outPose.mPosition.mult(originRot, outPose.mPosition);

		GHPoint3 pos;
		outPose.mPosition.getTranslate(pos);
		pos += originPos;
		outPose.mPosition.setTranslate(pos);
	}

	if (inPose.eTrackingResult == vr::TrackingResult_Running_OK
		|| inPose.eTrackingResult == vr::TrackingResult_Fallback_RotationOnly)
	{
		GHPoint3 linearVelocity;
		GHPoint3 angularVelocity;
		GHOpenVRUtil::convertToGHPoint3(inPose.vVelocity, linearVelocity);
		GHOpenVRUtil::convertToGHPoint3(inPose.vAngularVelocity, angularVelocity);

		float timePassed = mTimeVal.getTimePassed();
		float divTimePassed = GHFloat::isZero(timePassed) ? 0.0f : 1.0f / timePassed;

		outPose.mLinearAcceleration = linearVelocity;
		outPose.mLinearAcceleration -= outPose.mLinearVelocity;
		outPose.mLinearAcceleration *= divTimePassed;
		outPose.mLinearVelocity = linearVelocity;

		outPose.mAngularAcceleration = angularVelocity;
		outPose.mAngularAcceleration -= outPose.mAngularVelocity;
		outPose.mAngularAcceleration *= divTimePassed;
		outPose.mAngularVelocity = angularVelocity;


		originRot.multDir(outPose.mLinearAcceleration, outPose.mLinearAcceleration);
		originRot.multDir(outPose.mLinearVelocity, outPose.mLinearVelocity);
		outPose.mPosition.multDir(outPose.mAngularAcceleration, outPose.mAngularAcceleration);
		outPose.mPosition.multDir(outPose.mAngularVelocity, outPose.mAngularVelocity);
	}
}

const vr::TrackedDevicePose_t& GHOpenVRInputHandler::getCurrentHeadPose(void) const
{
	return mTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd];
}

static GHTransform createHandRotationTransform(void)
{
	GHTransform ret;
	ret.becomeXRotation(GHPI / 4.0f);
	return ret;
}

void GHOpenVRInputHandler::rotateHandPose(GHTransform& handTransform) const
{
	// Apparently SteamVR/OpenVR uses different cardinal axes than the Oculus API does for the controllers, at least on Rift
	//  see https://gitlab.com/znixian/OpenOVR/issues/76
	// Not sure how well this conversion works on other hardware yet, but this will make the Oculus Rift controller
	//  input behave the same way as through the Oculus API.

	static const GHTransform rotationTransform = createHandRotationTransform();
	rotationTransform.mult(handTransform, handTransform);
}

void GHOpenVRInputHandler::updatePoseables(void)
{
	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
	{
		const vr::TrackedDevicePose_t& pose = mTrackedDevicePose[i];
		if (!mVRSystem.getSystem()->IsTrackedDeviceConnected(i)) {
			continue;
		}

		if (!pose.bPoseIsValid)
		{
			continue;
		}

		vr::TrackedDeviceClass deviceClass = mVRSystem.getSystem()->GetTrackedDeviceClass(i);
		if (deviceClass == vr::TrackedDeviceClass_HMD)
		{
			GHInputStructs::Poseable& headPoseable = mInputState.getPoseableForUpdate(0);
			updatePose(pose, headPoseable);
		}
		else if (deviceClass == vr::TrackedDeviceClass_Controller)
		{
			vr::ETrackedControllerRole role = mVRSystem.getSystem()->GetControllerRoleForTrackedDeviceIndex(i);
			GHInputStructs::Poseable* hand = 0;
			GHInputStructs::Gamepad& gamepad = mInputState.getGamepadForUpdate(kGamepadIdx);
			mInputState.setGamepadOwner(kGamepadIdx, this);
			bool gamepadActive = mVRSystem.hmdIsActive();
			mInputState.handleGamepadActive(kGamepadIdx, gamepadActive, GHInputStructs::Gamepad::GPT_VRTOUCH);

			int handIdx = 0;
			if (role == vr::TrackedControllerRole_LeftHand)
			{
				hand = &gamepad.mPoseables[GHInputStructs::Gamepad::LEFT];
				handIdx = 0;
			}
			else if (role == vr::TrackedControllerRole_RightHand)
			{
				hand = &gamepad.mPoseables[GHInputStructs::Gamepad::RIGHT];
				handIdx = 1;
			}
			if (!hand) {
				continue;
			}
			hand->mActive = gamepadActive && (pose.eTrackingResult == vr::TrackingResult_Running_OK);
			updatePose(pose, *hand);
			rotateHandPose(hand->mPosition);

#ifdef GH_OPENVR_DEBUG_HANDS
			mDebugCross[handIdx].setTransform(hand->mPosition);
#endif
		}
	}
}

void GHOpenVRInputHandler::updateImpliedActions(void)
{
	// See if we can infer any additional input states from the ones hooked up to actions.
	// This is mainly to capture thumbup and point.
	GHInputStructs::Gamepad& gamepad = mInputState.getGamepadForUpdate(kGamepadIdx);

	// Infer finger point.
	float shouldRightPoint = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_RIGHTBUMPER) || gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_RIGHTBUMPER))
	{
		if (!gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_RIGHTTRIGGER) && !gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_RIGHTTRIGGER))
		{
			shouldRightPoint = 1.0f;
		}
	}
	gamepad.mButtons.setKeyState(GHKeyDef::POSE_GP_RIGHTINDEXPOINT, shouldRightPoint);

	float shouldLeftPoint = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_LEFTBUMPER) || gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_LEFTBUMPER))
	{
		if (!gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_LEFTTRIGGER) && !gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_LEFTTRIGGER))
		{
			shouldLeftPoint = 1.0f;
		}
	}
	gamepad.mButtons.setKeyState(GHKeyDef::POSE_GP_LEFTINDEXPOINT, shouldLeftPoint);

	// infer thumb up
	float shouldRightThumb = 1.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_A)) shouldRightThumb = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_B)) shouldRightThumb = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_A)) shouldRightThumb = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_B)) shouldRightThumb = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_RIGHTSTICKBUTTON)) shouldRightThumb = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_RIGHTTHUMBREST)) shouldRightThumb = 0.0f;
	gamepad.mButtons.setKeyState(GHKeyDef::POSE_GP_RIGHTTHUMBUP, shouldRightThumb);

	float shouldLeftThumb = 1.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_Y)) shouldLeftThumb = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_X)) shouldLeftThumb = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_Y)) shouldLeftThumb = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_X)) shouldLeftThumb = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_LEFTSTICKBUTTON)) shouldLeftThumb = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::TOUCH_GP_LEFTTHUMBREST)) shouldLeftThumb = 0.0f;
	gamepad.mButtons.setKeyState(GHKeyDef::POSE_GP_LEFTTHUMBUP, shouldLeftThumb);

	// infer back button (B/Y/LeftJoy/RightJoy)
	float shouldBackButton = 0.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_B)) shouldBackButton = 1.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_Y)) shouldBackButton = 1.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_LEFTSTICKBUTTON)) shouldBackButton = 1.0f;
	if (gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_RIGHTSTICKBUTTON)) shouldBackButton = 1.0f;
	mInputState.handleGamepadButtonChange(kGamepadIdx, GHKeyDef::KEY_GP_BACK, shouldBackButton);
}

GHOpenVRInputHandler::VrToGHActionMapping::VrToGHActionMapping(const char* name, GHKeyDef::Enum key, ActionType type, GHInputStructs::Gamepad::JoyIndex joyIndex)
	: mActionName(name)
	, mGHKey(key)
	, mActionType(type)
	, mJoyIndex(joyIndex)
{
	mVRAction = vr::k_ulInvalidActionHandle;
	vr::VRInput()->GetActionHandle(mActionName, &mVRAction);
}

GHOpenVRInputHandler::ActionManager::ActionManager(void)
{
	vr::VRInput()->GetActionSetHandle("/actions/main", &mActionSetMain);
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/LeftTrigger", GHKeyDef::KEY_GP_LEFTTRIGGER, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/RightTrigger", GHKeyDef::KEY_GP_RIGHTTRIGGER, VrToGHActionMapping::AT_BUTTON));

	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/lefthand_anim", (GHKeyDef::Enum)0, VrToGHActionMapping::AT_SKELETON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/righthand_anim", (GHKeyDef::Enum)0, VrToGHActionMapping::AT_SKELETON));

	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/RightTriggerTouch", GHKeyDef::TOUCH_GP_RIGHTTRIGGER, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/RightGrip", GHKeyDef::KEY_GP_RIGHTBUMPER, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/RightGripTouch", GHKeyDef::TOUCH_GP_RIGHTBUMPER, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/RightStickButton", GHKeyDef::KEY_GP_RIGHTSTICKBUTTON, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/RightStickTouch", GHKeyDef::TOUCH_GP_RIGHTSTICKBUTTON, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/A", GHKeyDef::KEY_GP_A, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/ATouch", GHKeyDef::TOUCH_GP_A, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/B", GHKeyDef::KEY_GP_B, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/BTouch", GHKeyDef::TOUCH_GP_B, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/LeftTriggerTouch", GHKeyDef::TOUCH_GP_LEFTTRIGGER, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/LeftGrip", GHKeyDef::KEY_GP_LEFTBUMPER, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/LeftGripTouch", GHKeyDef::TOUCH_GP_LEFTBUMPER, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/LeftStickButton", GHKeyDef::KEY_GP_LEFTSTICKBUTTON, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/LeftStickTouch", GHKeyDef::TOUCH_GP_LEFTSTICKBUTTON, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/Y", GHKeyDef::KEY_GP_Y, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/YTouch", GHKeyDef::TOUCH_GP_Y, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/X", GHKeyDef::KEY_GP_X, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/XTouch", GHKeyDef::TOUCH_GP_Y, VrToGHActionMapping::AT_BUTTON));

	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/LeftMenu", GHKeyDef::KEY_GP_LEFTMENU, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/RightMenu", GHKeyDef::KEY_GP_RIGHTMENU, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/LeftThumbrestTouch", GHKeyDef::TOUCH_GP_LEFTTHUMBREST, VrToGHActionMapping::AT_BUTTON));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/RightThumbrestTouch", GHKeyDef::TOUCH_GP_RIGHTTHUMBREST, VrToGHActionMapping::AT_BUTTON));

	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/RightStick", (GHKeyDef::Enum)0, VrToGHActionMapping::AT_BUTTON, GHInputStructs::Gamepad::RIGHT));
	mActions.push_back(GHOpenVRInputHandler::VrToGHActionMapping("/actions/main/in/LeftStick", (GHKeyDef::Enum)0, VrToGHActionMapping::AT_BUTTON, GHInputStructs::Gamepad::LEFT));
}

// from https://github.com/ValveSoftware/openvr/blob/master/samples/hellovr_opengl/hellovr_opengl_main.cpp
bool GHOpenVRInputHandler::ActionManager::getDigitalActionState(vr::VRActionHandle_t action, vr::VRInputValueHandle_t* pDevicePath)
{
	vr::InputDigitalActionData_t actionData;
	vr::EVRInputError getActionErr = vr::VRInput()->GetDigitalActionData(action, &actionData, sizeof(actionData), vr::k_ulInvalidInputValueHandle);
	if (getActionErr != vr::EVRInputError::VRInputError_None)
	{
		return false;
	}
	if (pDevicePath)
	{
		*pDevicePath = vr::k_ulInvalidInputValueHandle;
		if (actionData.bActive)
		{
			vr::InputOriginInfo_t originInfo;
			if (vr::VRInputError_None == vr::VRInput()->GetOriginTrackedDeviceInfo(actionData.activeOrigin, &originInfo, sizeof(originInfo)))
			{
				*pDevicePath = originInfo.devicePath;
			}
		}
	}
	return actionData.bActive && actionData.bState;
}

bool GHOpenVRInputHandler::ActionManager::getAnalogActionState(vr::VRActionHandle_t action, GHPoint3& outPos)
{
	vr::InputAnalogActionData_t actionData;
	vr::EVRInputError getActionErr = vr::VRInput()->GetAnalogActionData(action, &actionData, sizeof(vr::InputAnalogActionData_t), vr::k_ulInvalidInputValueHandle);
	if (getActionErr != vr::EVRInputError::VRInputError_None)
	{
		return false;
	}
	if (!actionData.bActive)
	{
		return false;
	}
	outPos[0] = actionData.x;
	outPos[1] = actionData.y;
	outPos[2] = actionData.z;
	return true;
}

void GHOpenVRInputHandler::ActionManager::updateActions(GHInputState& inputState)
{
	// tell openvr to update the action sets
	vr::VRActiveActionSet_t actionSet = { 0 };
	actionSet.ulActionSet = mActionSetMain;
	vr::VRInput()->UpdateActionState(&actionSet, sizeof(actionSet), 1);

	// and query the state of each action
	for (unsigned int i = 0; i < mActions.size(); ++i)
	{
		if (mActions[i].mActionType == VrToGHActionMapping::AT_BUTTON)
		{
			bool actionState = getDigitalActionState(mActions[i].mVRAction);
			float actionFloat = actionState ? 1.0f : 0.0f;
			inputState.handleGamepadButtonChange(1, mActions[i].mGHKey, actionFloat);
		}
		else if (mActions[i].mActionType == VrToGHActionMapping::AT_SKELETON)
		{
			updateSkeletonAction(mActions[i]);
		}
		else if (mActions[i].mActionType == VrToGHActionMapping::AT_JOYSTICK)
		{
			GHPoint3 joyPos;
			getAnalogActionState(mActions[i].mVRAction, joyPos);
			inputState.handleGamepadJoystickChange(kGamepadIdx, mActions[i].mJoyIndex, GHPoint2(joyPos[0], joyPos[1]));
		}
	}
}

void GHOpenVRInputHandler::ActionManager::updateSkeletonAction(const VrToGHActionMapping& action)
{
	// todo: figure out where this data should go
	// I imagine we'll pass in a handle to that gh target into VrToGHActionMapping constructor.

	vr::InputSkeletalActionData_t skeletalActionData;
	vr::EVRInputError getSkeletalActionErr = vr::VRInput()->GetSkeletalActionData(action.mVRAction, &skeletalActionData, sizeof(skeletalActionData));
	if (getSkeletalActionErr != vr::EVRInputError::VRInputError_None)
	{
		GHDebugMessage::outputString("Failed to get skeletal action data");
		return;
	}
	if (!skeletalActionData.bActive)
	{
		return;
	}
	// skeletalActionData has an origin.

	// can probably hardcode boneCount to 31.
	uint32_t boneCount = 0;
	vr::EVRInputError boneCountErr = vr::VRInput()->GetBoneCount(action.mVRAction, &boneCount);
	if (boneCountErr != vr::EVRInputError::VRInputError_None)
	{
		GHDebugMessage::outputString("Failed to get bone count");
		return;
	}
	vr::BoneIndex_t* boneParents = new vr::BoneIndex_t[boneCount];
	vr::EVRInputError boneParentsErr = vr::VRInput()->GetBoneHierarchy(action.mVRAction, boneParents, boneCount);
	if (boneParentsErr != vr::EVRInputError::VRInputError_None)
	{
		GHDebugMessage::outputString("Failed to get bone hierarchy");
		delete [] boneParents;
		return;
	}
	vr::VRBoneTransform_t* boneVrTransforms = new vr::VRBoneTransform_t[boneCount];
	vr::EVRInputError transformsError = vr::VRInput()->GetSkeletalBoneData(action.mVRAction,
		vr::EVRSkeletalTransformSpace::VRSkeletalTransformSpace_Model,
		vr::EVRSkeletalMotionRange::VRSkeletalMotionRange_WithController,
		boneVrTransforms,
		boneCount);
	if (transformsError != vr::EVRInputError::VRInputError_None)
	{
		GHDebugMessage::outputString("Failed to get bone transforms");
	}
	// todo: do something with this data
	delete[] boneParents;
	delete[] boneVrTransforms;
}
