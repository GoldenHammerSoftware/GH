#include "GHInputStateOculusAvatarPoser.h"
#include "GHInputState.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHOculusAvatarUtil.h"
#include "GHKeyDef.h"
#include "GHMath/GHTransform.h"

namespace
{
	void convertGHToAvatarTransform(const GHTransform& ghTrans, ovrAvatarTransform& outTrans)
	{
		GHOculusUtil::convertGHTransformToOvrAvatar(ghTrans, outTrans);
	}

	// Value mapping taken from GHOculusTouchInputTranslator. If that changes than this will need to change too.
	void fillHandInput(const GHInputState& inputState, unsigned int gamepadIndex, unsigned int handIndex, const ovrAvatarTransform& transform, ovrAvatarHandInputState& outState)
	{
		outState.transform = transform;
		outState.buttonMask = 0;
		outState.touchMask = 0;
		outState.joystickX = inputState.getGamepad(gamepadIndex).mJoySticks[handIndex][0];
		outState.joystickY = inputState.getGamepad(gamepadIndex).mJoySticks[handIndex][1];
		outState.isActive = inputState.getGamepad(gamepadIndex).mPoseables[handIndex].mActive;
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

			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_LEFTMENU)) {
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
			if (inputState.getGamepadKeyState(gamepadIndex, GHKeyDef::KEY_GP_RIGHTMENU)) {
				outState.buttonMask |= ovrAvatarButton_Three;
			}

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


GHInputStateOculusAvatarPoser::GHInputStateOculusAvatarPoser(const GHTransform& hmdOrigin, const GHInputState& inputState, const GHTimeVal& timeVal)
	: mHMDOrigin(hmdOrigin)
	, mInputState(inputState)
	, mTimeVal(timeVal)
{

}

void GHInputStateOculusAvatarPoser::poseAvatar(ovrAvatar* avatar)
{
	if (!avatar) { return; }

	//todo: pass in
	const int mTouchControllerIndex = 1;
	const int mHeadPoseableIndex = 0;

	const GHInputStructs::Gamepad& gamepad = mInputState.getGamepad(mTouchControllerIndex);

	//This part is a bit silly:
	// GHOculusAvatar is expecting transforms in HMD local space -- it will transform them into worldspace using the HMD Origin.
	//  However, the GHInputState transforms are already in world space (converted from HMD space in the input handling).
	//  So we have to transform them back to HMD space here.
	//Three options:
	//1) pass a flag to GHOculusAvatar telling it that it may accept worldspace transforms
	//2) make it always accept worldspace transforms and move all worldspace conversions into the AvatarPosers
	//3) Accept that the this version is always going to be less efficient (it is anyway) and only use it for development,
	//		eventually implementing a direct-from-platform-input version.
	GHTransform hmdInv = mHMDOrigin;
	hmdInv.invert();

	GHTransform headPos = mInputState.getPoseable(mHeadPoseableIndex).mPosition;
	GHTransform leftHandPos = gamepad.mPoseables[0].mPosition;
	GHTransform rightHandPos = gamepad.mPoseables[1].mPosition;

	headPos.mult(hmdInv, headPos);
	leftHandPos.mult(hmdInv, leftHandPos);
	rightHandPos.mult(hmdInv, rightHandPos);

	ovrAvatarTransform hmdTrans, leftTrans, rightTrans;
	convertGHToAvatarTransform(headPos, hmdTrans);
	convertGHToAvatarTransform(leftHandPos, leftTrans);
	convertGHToAvatarTransform(rightHandPos, rightTrans);

	ovrAvatarHandInputState leftInput, rightInput;
	fillHandInput(mInputState, mTouchControllerIndex, 0, leftTrans, leftInput);
	fillHandInput(mInputState, mTouchControllerIndex, 1, rightTrans, rightInput);

	//todo: microphone input
	//ovrAvatarPose_UpdateVoiceVisualization

	float deltaSeconds = mTimeVal.getTimePassed();

	ovrAvatarPose_UpdateBody(avatar, hmdTrans);
	ovrAvatarPose_UpdateHands(avatar, leftInput, rightInput);
	ovrAvatarPose_Finalize(avatar, deltaSeconds);
}

GHInputStateOculusAvatarPoserFactory::GHInputStateOculusAvatarPoserFactory(const GHTransform& hmdOrigin, const GHInputState& inputState, const GHTimeVal& timeVal)
	: mHMDOrigin(hmdOrigin)
	, mInputState(inputState)
	, mTimeVal(timeVal)
{

}

GHInputStateOculusAvatarPoserFactory::~GHInputStateOculusAvatarPoserFactory(void)
{

}

GHInputStateOculusAvatarPoser* GHInputStateOculusAvatarPoserFactory::createAvatarPoser(void)
{
	return new GHInputStateOculusAvatarPoser(mHMDOrigin, mInputState, mTimeVal);
}
