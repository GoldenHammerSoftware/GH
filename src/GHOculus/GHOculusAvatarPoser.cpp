#include "GHOculusAvatarPoser.h"
#include "GHOculusSystemController.h"
#include "GHOculusAvatarUtil.h"

GHOculusAvatarPoser::GHOculusAvatarPoser(const GHOculusSystemController& oculusSystem)
	: mOculusSystem(oculusSystem)
{

}

void GHOculusAvatarPoser::poseAvatar(ovrAvatar* avatar) 
{
	if (!avatar) { return; }

	const ovrTrackingState& trackingState = mOculusSystem.getTrackingState();
	const ovrInputState& inputState = mOculusSystem.getInputState(GHOculusSystemController::InputState::GI_TOUCH).mOvrInput;

	ovrAvatarTransform hmdTrans, leftTrans, rightTrans;
	convertPoseToAvatarTransform(trackingState.HeadPose.ThePose, hmdTrans);
	convertPoseToAvatarTransform(trackingState.HandPoses[0].ThePose, leftTrans);
	convertPoseToAvatarTransform(trackingState.HandPoses[1].ThePose, rightTrans);

	ovrAvatarHandInputState leftInput, rightInput;
	convertOvrInputToAvatarInput(leftTrans, inputState, ovrHand_Left, leftInput);
	convertOvrInputToAvatarInput(rightTrans, inputState, ovrHand_Right, rightInput);

	//todo: microphone input
	//ovrAvatarPose_UpdateVoiceVisualization

	double deltaSeconds = inputState.TimeInSeconds;
	deltaSeconds -= mLastUpdateTime;

	ovrAvatarPose_UpdateBody(avatar, hmdTrans);
	ovrAvatarPose_UpdateHands(avatar, leftInput, rightInput);
	ovrAvatarPose_Finalize(avatar, (float)deltaSeconds);

	mLastUpdateTime = inputState.TimeInSeconds;
}

void GHOculusAvatarPoser::convertPoseToAvatarTransform(const ovrPosef& inPose, ovrAvatarTransform& outTrans) const
{
	outTrans.orientation.x = inPose.Orientation.x;
	outTrans.orientation.y = inPose.Orientation.y;
	outTrans.orientation.z = inPose.Orientation.z;
	outTrans.orientation.w = inPose.Orientation.w;

	outTrans.position.x = inPose.Position.x;
	outTrans.position.y = inPose.Position.y;
	outTrans.position.z = inPose.Position.z;

	outTrans.scale.x = 1.f;
	outTrans.scale.y = 1.f;
	outTrans.scale.z = 1.f;
}

void GHOculusAvatarPoser::convertOvrInputToAvatarInput(const ovrAvatarTransform& transform, const ovrInputState& inputState, ovrHandType hand, ovrAvatarHandInputState& outState) const
{
	outState.transform = transform;
	outState.buttonMask = 0;
	outState.touchMask = 0;
	outState.joystickX = inputState.Thumbstick[hand].x;
	outState.joystickY = inputState.Thumbstick[hand].y;
	outState.indexTrigger = inputState.IndexTrigger[hand];
	outState.handTrigger = inputState.HandTrigger[hand];
	outState.isActive = false;
	if (hand == ovrHand_Left)
	{
		if (inputState.Buttons & ovrButton_X) outState.buttonMask |= ovrAvatarButton_One;
		if (inputState.Buttons & ovrButton_Y) outState.buttonMask |= ovrAvatarButton_Two;
		if (inputState.Buttons & ovrButton_Enter) outState.buttonMask |= ovrAvatarButton_Three;
		if (inputState.Buttons & ovrButton_LThumb) outState.buttonMask |= ovrAvatarButton_Joystick;
		if (inputState.Touches & ovrTouch_X) outState.touchMask |= ovrAvatarTouch_One;
		if (inputState.Touches & ovrTouch_Y) outState.touchMask |= ovrAvatarTouch_Two;
		if (inputState.Touches & ovrTouch_LThumb) outState.touchMask |= ovrAvatarTouch_Joystick;
		if (inputState.Touches & ovrTouch_LThumbRest) outState.touchMask |= ovrAvatarTouch_ThumbRest;
		if (inputState.Touches & ovrTouch_LIndexTrigger) outState.touchMask |= ovrAvatarTouch_Index;
		if (inputState.Touches & ovrTouch_LIndexPointing) outState.touchMask |= ovrAvatarTouch_Pointing;
		if (inputState.Touches & ovrTouch_LThumbUp) outState.touchMask |= ovrAvatarTouch_ThumbUp;
		outState.isActive = (inputState.ControllerType & ovrControllerType_LTouch) != 0;
	}
	else if (hand == ovrHand_Right)
	{
		if (inputState.Buttons & ovrButton_A) outState.buttonMask |= ovrAvatarButton_One;
		if (inputState.Buttons & ovrButton_B) outState.buttonMask |= ovrAvatarButton_Two;
		if (inputState.Buttons & ovrButton_Home) outState.buttonMask |= ovrAvatarButton_Three;
		if (inputState.Buttons & ovrButton_RThumb) outState.buttonMask |= ovrAvatarButton_Joystick;
		if (inputState.Touches & ovrTouch_A) outState.touchMask |= ovrAvatarTouch_One;
		if (inputState.Touches & ovrTouch_B) outState.touchMask |= ovrAvatarTouch_Two;
		if (inputState.Touches & ovrTouch_RThumb) outState.touchMask |= ovrAvatarTouch_Joystick;
		if (inputState.Touches & ovrTouch_RThumbRest) outState.touchMask |= ovrAvatarTouch_ThumbRest;
		if (inputState.Touches & ovrTouch_RIndexTrigger) outState.touchMask |= ovrAvatarTouch_Index;
		if (inputState.Touches & ovrTouch_RIndexPointing) outState.touchMask |= ovrAvatarTouch_Pointing;
		if (inputState.Touches & ovrTouch_RThumbUp) outState.touchMask |= ovrAvatarTouch_ThumbUp;
		outState.isActive = (inputState.ControllerType & ovrControllerType_RTouch) != 0;
	}
}

GHOculusAvatarPoserFactory::GHOculusAvatarPoserFactory(const GHOculusSystemController& oculusSystem)
	: mOculusSystem(oculusSystem)
{

}

GHOculusAvatarPlatformPoser* GHOculusAvatarPoserFactory::createAvatarPoser(void)
{
	return new GHOculusAvatarPoser(mOculusSystem);
}
