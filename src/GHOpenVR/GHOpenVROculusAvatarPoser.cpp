#include "GHOpenVROculusAvatarPoser.h"
#include "GHOculusAvatarUtil.h"
#include "GHInputState.h"
#include "GHKeyDef.h"
#include "GHMath/GHTransform.h"

GHOpenVROculusAvatarPoser::GHOpenVROculusAvatarPoser(GHInputState& inputState)
	: mInputState(inputState)
{
}

namespace
{
	//TODO: Fill this out based on OpenVR input
	// For now, just place the hands in the world where we can see them (coordinates are in Oculus space).
	void fillHandInput(const GHInputState& inputState, unsigned int gamepadIndex, unsigned int handIndex, const ovrAvatarTransform& transform, ovrAvatarHandInputState& outState)
	{
		memset(&outState, 0, sizeof(outState));
		outState.transform = transform;
		outState.isActive = inputState.getGamepad(gamepadIndex).mPoseables[handIndex].mActive;

		outState.transform.position.z = -1.0f;
		outState.transform.position.y = 1.0f;
		if (handIndex == 1)
		{
			outState.transform.position.x = .5f;
		}

		outState.buttonMask = 0;
		outState.touchMask = 0;
		outState.joystickX = inputState.getGamepad(gamepadIndex).mJoySticks[handIndex][0];
		outState.joystickY = inputState.getGamepad(gamepadIndex).mJoySticks[handIndex][1];
		if (handIndex == 0)
		{
			outState.indexTrigger = inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_LEFTTRIGGER);
			outState.handTrigger = inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_LEFTBUMPER);

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_X)) {
				outState.buttonMask |= ovrAvatarButton_One;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_Y)) {
				outState.buttonMask |= ovrAvatarButton_Two;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_BACK)) {
				outState.buttonMask |= ovrAvatarButton_Three;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_LEFTSTICKBUTTON)) {
				outState.buttonMask |= ovrAvatarButton_Joystick;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::TOUCH_GP_X)) {
				outState.touchMask |= ovrAvatarTouch_One;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::TOUCH_GP_Y)) {
				outState.touchMask |= ovrAvatarTouch_Two;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::TOUCH_GP_LEFTSTICKBUTTON)) {
				outState.touchMask |= ovrAvatarTouch_Joystick;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::TOUCH_GP_LEFTTHUMBREST)) {
				outState.touchMask |= ovrAvatarTouch_ThumbRest;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::TOUCH_GP_LEFTBUMPER)) {
				outState.touchMask |= ovrAvatarTouch_Index;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::POSE_GP_LEFTINDEXPOINT)) {
				outState.touchMask |= ovrAvatarTouch_Pointing;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::POSE_GP_LEFTTHUMBUP)) {
				outState.touchMask |= ovrAvatarTouch_ThumbUp;
			}
		}
		else if (handIndex == 1)
		{
			outState.indexTrigger = inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_RIGHTTRIGGER);
			outState.handTrigger = inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_RIGHTBUMPER);

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_A)) {
				outState.buttonMask |= ovrAvatarButton_One;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_B)) {
				outState.buttonMask |= ovrAvatarButton_Two;
			}

			// This button (ovrButton_Home in Oculus input SDK) is not mapped in GHOculusTouchInputTranslator
			//if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::)) {
			//	outState.buttonMask |= ovrAvatarButton_Three;
			//}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_RIGHTSTICKBUTTON)) {
				outState.buttonMask |= ovrAvatarButton_Joystick;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::TOUCH_GP_A)) {
				outState.touchMask |= ovrAvatarTouch_One;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::TOUCH_GP_B)) {
				outState.touchMask |= ovrAvatarTouch_Two;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::TOUCH_GP_RIGHTSTICKBUTTON)) {
				outState.touchMask |= ovrAvatarTouch_Joystick;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::TOUCH_GP_RIGHTTHUMBREST)) {
				outState.touchMask |= ovrAvatarTouch_ThumbRest;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::TOUCH_GP_RIGHTBUMPER)) {
				outState.touchMask |= ovrAvatarTouch_Index;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::POSE_GP_RIGHTINDEXPOINT)) {
				outState.touchMask |= ovrAvatarTouch_Pointing;
			}

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::POSE_GP_RIGHTTHUMBUP)) {
				outState.touchMask |= ovrAvatarTouch_ThumbUp;
			}
		}
	}
}

void GHOpenVROculusAvatarPoser::poseAvatar(ovrAvatar* avatar)
{
	if (!avatar) { return; }

	GHTransform identityTrans;
	identityTrans.becomeIdentity();

	ovrAvatarTransform hmdTrans, leftTrans, rightTrans;
	GHOculusUtil::convertGHTransformToOvrAvatar(identityTrans, hmdTrans);
	GHOculusUtil::convertGHTransformToOvrAvatar(identityTrans, leftTrans);
	GHOculusUtil::convertGHTransformToOvrAvatar(identityTrans, rightTrans);

	ovrAvatarHandInputState leftInput, rightInput;
	fillHandInput(mInputState, 1, 0, leftTrans, leftInput);
	fillHandInput(mInputState, 1, 1, rightTrans, rightInput);

	//todo: microphone input
	//ovrAvatarPose_UpdateVoiceVisualization

	//Todo: get this from either a GHTimeVal or the OpenVR API.
	float deltaSeconds = 1.0 / 60.f;

	ovrAvatarPose_UpdateBody(avatar, hmdTrans);
	ovrAvatarPose_UpdateHands(avatar, leftInput, rightInput);
	ovrAvatarPose_Finalize(avatar, deltaSeconds);
}

GHOpenVROculusAvatarPoserFactory::GHOpenVROculusAvatarPoserFactory(GHInputState& inputState)
	: mInputState(inputState)
{
}

GHOpenVROculusAvatarPoserFactory::~GHOpenVROculusAvatarPoserFactory(void)
{
}

GHOpenVROculusAvatarPoser* GHOpenVROculusAvatarPoserFactory::createAvatarPoser(void)
{
	return new GHOpenVROculusAvatarPoser(mInputState);
}
