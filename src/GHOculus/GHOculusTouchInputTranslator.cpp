#include "GHOculusTouchInputTranslator.h"
#include "GHOculusSystemController.h"
#include "GHOculusUtil.h"
#include "GHInputState.h"
#include "GHKeyDef.h"
#include "GHMath/GHFloat.h"

#include "GHDebugDraw.h"
DebugCross leftCross, rightCross;

GHOculusTouchInputTranslator::GHOculusTouchInputTranslator(const GHTransform& hmdOrigin,
														   const GHOculusSystemController& oculusSystem,
														   GHInputState& inputState)
	: mHMDOrigin(hmdOrigin)
	, mOculusSystem(oculusSystem)
	, mInputState(inputState)
{

}

void GHOculusTouchInputTranslator::onActivate(void)
{

}

void GHOculusTouchInputTranslator::onDeactivate(void)
{

}
 
void GHOculusTouchInputTranslator::handleButtonPress(unsigned int ghGPIndex, const ovrInputState& ovrInput, unsigned int ghKey, unsigned int ovrKey)
{
	handleButton(ghGPIndex, ovrInput.Buttons, ghKey, ovrKey);
}

void GHOculusTouchInputTranslator::handleButtonTouch(unsigned int ghGPIndex, const ovrInputState& ovrInput, unsigned int ghKey, unsigned int ovrKey)
{
	handleButton(ghGPIndex, ovrInput.Touches, ghKey, ovrKey);
}

void GHOculusTouchInputTranslator::handleButton(unsigned int ghGPIndex, const unsigned int ovrBitmask, unsigned int ghKey, unsigned int ovrKey)
{
	mInputState.handleGamepadButtonChange(ghGPIndex, ghKey, (ovrBitmask & ovrKey) ? 1.f : 0.f );
}

void GHOculusTouchInputTranslator::handleNonBinaryButton(unsigned int ghGPIndex, unsigned int ghKey, float ovrValue)
{
	if (GHFloat::isZero(ovrValue))
	{
		ovrValue = 0.f;
	}

	mInputState.handleGamepadButtonChange(ghGPIndex, ghKey, ovrValue);
}

void GHOculusTouchInputTranslator::update(void)
{
	GHInputStructs::Poseable& ghHeadPose = mInputState.getPoseableForUpdate(0);
	GHInputStructs::Poseable& ghLeftPose = mInputState.getGamepadForUpdate(1).mPoseables[0];
	GHInputStructs::Poseable& ghRightPose = mInputState.getGamepadForUpdate(1).mPoseables[1];
	
	const ovrTrackingState& trackingState = mOculusSystem.getTrackingState();
	updatePose(trackingState.HeadPose, ghHeadPose);
	updatePose(trackingState.HandPoses[ovrEye_Left], ghLeftPose);
	updatePose(trackingState.HandPoses[ovrEye_Right], ghRightPose);

	//debug draw
	if (0)
	{	
		leftCross.setSizes(0.1f,0.1f,0.1f);
		leftCross.setTransform(ghLeftPose.mPosition);
		rightCross.setSizes(0.1f, 0.1f, 0.1f);
		rightCross.setTransform(ghRightPose.mPosition);
	}

	const GHOculusSystemController::InputState& touchState = mOculusSystem.getInputState(GHOculusSystemController::InputState::GI_TOUCH);
	const GHOculusSystemController::InputState& gamepadState = mOculusSystem.getInputState(GHOculusSystemController::InputState::GI_CONTROLLER);
	const GHOculusSystemController::InputState& remoteState = mOculusSystem.getInputState(GHOculusSystemController::InputState::GI_REMOTE);

	// claim the gamepads for oculus otherwise xinput will come through and clobber us.
	mInputState.setGamepadOwner(touchState.mGamepadIndex, this);
	mInputState.setGamepadOwner(gamepadState.mGamepadIndex, this);
	mInputState.setGamepadOwner(remoteState.mGamepadIndex, this);

	mInputState.handleGamepadActive(touchState.mGamepadIndex, touchState.mIsActive, GHInputStructs::Gamepad::GPT_VRTOUCH);
	mInputState.handleGamepadActive(gamepadState.mGamepadIndex, gamepadState.mIsActive);
	mInputState.handleGamepadActive(remoteState.mGamepadIndex, remoteState.mIsActive);

	//touch controls
	if (touchState.mIsActive)
	{
		unsigned int gpIndex = touchState.mGamepadIndex;
		const ovrInputState& ovrInput = touchState.mOvrInput;

		handleInputCommonToTouchAndGamepad(gpIndex, ovrInput);

		//on touch controller, these are nonbinary buttons
		handleNonBinaryButton(gpIndex, GHKeyDef::KEY_GP_LEFTTRIGGER, ovrInput.IndexTrigger[ovrHand_Left]);
		handleNonBinaryButton(gpIndex, GHKeyDef::KEY_GP_RIGHTTRIGGER, ovrInput.IndexTrigger[ovrHand_Right]);
		handleNonBinaryButton(gpIndex, GHKeyDef::KEY_GP_LEFTBUMPER, ovrInput.HandTrigger[ovrHand_Left]);
		handleNonBinaryButton(gpIndex, GHKeyDef::KEY_GP_RIGHTBUMPER, ovrInput.HandTrigger[ovrHand_Right]);

		//only the touch controller has touches
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::TOUCH_GP_A, ovrTouch_A);
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::TOUCH_GP_B, ovrTouch_B);
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::TOUCH_GP_X, ovrTouch_X);
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::TOUCH_GP_Y, ovrTouch_Y);
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::TOUCH_GP_LEFTBUMPER, ovrTouch_LIndexTrigger);
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::TOUCH_GP_RIGHTBUMPER, ovrTouch_RIndexTrigger);
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::TOUCH_GP_LEFTTHUMBREST, ovrTouch_LThumbRest);
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::TOUCH_GP_RIGHTTHUMBREST, ovrTouch_RThumbRest);
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::POSE_GP_LEFTINDEXPOINT, ovrTouch_LIndexPointing);
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::POSE_GP_RIGHTINDEXPOINT, ovrTouch_RIndexPointing);
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::POSE_GP_LEFTTHUMBUP, ovrTouch_LThumbUp);
		handleButtonTouch(gpIndex, ovrInput, GHKeyDef::POSE_GP_RIGHTTHUMBUP, ovrTouch_RThumbUp);
	}
	// todo: check each touch controller individually.
	// don't make the poses active unless the headset is on.
	ghLeftPose.mActive = touchState.mIsActive && mOculusSystem.isVisible();
	ghRightPose.mActive = touchState.mIsActive && mOculusSystem.isVisible();

	//gamepad controls
	if (gamepadState.mIsActive)
	{
		unsigned int gpIndex = gamepadState.mGamepadIndex;
		const ovrInputState& ovrInput = gamepadState.mOvrInput;

		handleInputCommonToTouchAndGamepad(gpIndex, ovrInput);

		//on gamepad, these are binary buttons
		handleButtonPress(gpIndex, ovrInput, GHKeyDef::KEY_GP_LEFTBUMPER, ovrButton_LShoulder);
		handleButtonPress(gpIndex, ovrInput, GHKeyDef::KEY_GP_RIGHTBUMPER, ovrButton_RShoulder);

		//gamepad-only button
		handleButtonPress(gpIndex, ovrInput, GHKeyDef::KEY_GP_BACK, ovrButton_Back);

		handleInputCommonToGamepadAndRemote(gpIndex, ovrInput);
	}

	//remote controls
	if (remoteState.mIsActive)
	{
		unsigned int gpIndex = remoteState.mGamepadIndex;
		const ovrInputState& ovrInput = remoteState.mOvrInput;

		handleInputCommonToGamepadAndRemote(gpIndex, ovrInput);

		//according to the header documentation
		handleButtonPress(gpIndex, ovrInput, GHKeyDef::KEY_GP_A, ovrButton_A); //Select button on Oculus Remote.
		handleButtonPress(gpIndex, ovrInput, GHKeyDef::KEY_GP_B, ovrButton_B); //Back button on Oculus Remote.
	}
}

void GHOculusTouchInputTranslator::handleInputCommonToTouchAndGamepad(unsigned int ghGPIndex, const ovrInputState& ovrInput)
{
	//these are common to touch controllers and gamepad
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_A, ovrButton_A);
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_B, ovrButton_B);
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_X, ovrButton_X);
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_Y, ovrButton_Y);
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_LEFTSTICKBUTTON, ovrButton_LThumb);
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_RIGHTSTICKBUTTON, ovrButton_RThumb);

	// ovrButton_Enter is meant to be used as the start button on an xbox controller.
	// we're translating it to be gp_back instead of gp_start so that it matches the oculus button without having to edit a bunch of guis to support both back and start.
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_BACK, ovrButton_Enter);
	// double count that as left menu
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_LEFTMENU, ovrButton_Enter);

	//trigger buttons: common between touch and gamepad
	handleNonBinaryButton(ghGPIndex, GHKeyDef::KEY_GP_LEFTTRIGGER, ovrInput.IndexTrigger[ovrHand_Left]);
	handleNonBinaryButton(ghGPIndex, GHKeyDef::KEY_GP_RIGHTTRIGGER, ovrInput.IndexTrigger[ovrHand_Right]);
	handleNonBinaryButton(ghGPIndex, GHKeyDef::KEY_GP_LEFTBUMPER, ovrInput.HandTrigger[ovrHand_Left]);
	handleNonBinaryButton(ghGPIndex, GHKeyDef::KEY_GP_RIGHTBUMPER, ovrInput.HandTrigger[ovrHand_Right]);

	//joysticks: common between touch and gamepad
	GHPoint2 leftJoystick(ovrInput.Thumbstick[ovrHand_Left].x, ovrInput.Thumbstick[ovrHand_Left].y);
	GHPoint2 rightJoystick(ovrInput.Thumbstick[ovrHand_Right].x, ovrInput.Thumbstick[ovrHand_Right].y);
	mInputState.handleGamepadJoystickChange(ghGPIndex, 0, leftJoystick);
	mInputState.handleGamepadJoystickChange(ghGPIndex, 1, rightJoystick);
}

void GHOculusTouchInputTranslator::handleInputCommonToGamepadAndRemote(unsigned int ghGPIndex, const ovrInputState& ovrInput)
{
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_DPADUP, ovrButton_Up);
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_DPADDOWN, ovrButton_Down);
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_DPADLEFT, ovrButton_Left);
	handleButtonPress(ghGPIndex, ovrInput, GHKeyDef::KEY_GP_DPADRIGHT, ovrButton_Right);
}

void GHOculusTouchInputTranslator::updatePose(const ovrPoseStatef& inPose, GHInputStructs::Poseable& outPose)
{
	GHOculusUtil::convertOvrPoseToGHTransform(inPose.ThePose, outPose.mPosition);

	GHPoint3 originPos;
	mHMDOrigin.getTranslate(originPos);

	GHTransform originRot = mHMDOrigin;
	originRot.setTranslate(GHPoint3(0, 0, 0));
	outPose.mPosition.mult(originRot, outPose.mPosition);

	GHPoint3 pos;
	outPose.mPosition.getTranslate(pos);
	pos *= mOculusSystem.getOculusToGameScale();
	pos += originPos;
	outPose.mPosition.setTranslate(pos);

	GHOculusUtil::convertOvrVecToGH(inPose.AngularAcceleration, outPose.mAngularAcceleration);
	GHOculusUtil::convertOvrVecToGH(inPose.AngularVelocity, outPose.mAngularVelocity);
	GHOculusUtil::convertOvrVecToGH(inPose.LinearAcceleration, outPose.mLinearAcceleration);
	GHOculusUtil::convertOvrVecToGH(inPose.LinearVelocity, outPose.mLinearVelocity);

	originRot.multDir(outPose.mLinearAcceleration, outPose.mLinearAcceleration);
	originRot.multDir(outPose.mLinearVelocity, outPose.mLinearVelocity);
	outPose.mPosition.multDir(outPose.mAngularAcceleration, outPose.mAngularAcceleration);
	outPose.mPosition.multDir(outPose.mAngularVelocity, outPose.mAngularVelocity);
}
