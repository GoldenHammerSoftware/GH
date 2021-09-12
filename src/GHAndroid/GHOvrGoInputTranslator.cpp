#ifdef GH_OVR_GO

#include "GHOvrGoInputTranslator.h"
#include "GHOvrGoInputState.h"
#include "GHOvrGoFrameState.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOvrGoUtil.h"

#include "GHDebugDraw.h"
DebugCross handCross;

GHOvrGoInputTranslator::GHOvrGoInputTranslator(const GHOvrGoFrameState& frameState,
	GHInputState& ghInputState, const GHOvrGoInputState& ovrInputState,
	const GHTransform& hmdOrigin)
: mGHInputState(ghInputState)
, mOvrInputState(ovrInputState)
, mHMDOrigin(hmdOrigin)
, mFrameState(frameState)
{
}

void GHOvrGoInputTranslator::update(void)
{
	updateHead();
	updateHand();
}

void GHOvrGoInputTranslator::updateHead(void)
{
	GHInputStructs::Poseable& ghHeadPose = mGHInputState.getPoseableForUpdate(0);

	const GHOvrGoInputState::OvrGoInputDevice* headDevice = mOvrInputState.getDevice(GHOvrGoInputState::GI_HEADSET);
	if (headDevice && headDevice->isActive() && headDevice->getTracking())
	{
		const ovrTracking * hmdTracking = headDevice->getTracking();
		updatePose(hmdTracking->HeadPose, ghHeadPose);
	}
	else
	{
		updatePose(mFrameState.mPredictedTracking.HeadPose, ghHeadPose);
	}
	ghHeadPose.mActive = true;
}

void GHOvrGoInputTranslator::updateHand(void)
{
	const GHOvrGoInputState::OvrGoInputDevice* handDevice = mOvrInputState.getDevice(GHOvrGoInputState::GI_CONTROLLER);
	if (!handDevice)
	{
		return;
	}
	const int GHGoGamepadIndex = 1;

	bool isLeftHand = false;
	int handIndex = 1;
	const ovrInputTrackedRemoteCapabilities* capabilities = handDevice->getTrackedRemoteCapabilities();
	GHPoint2 trackpadRange(1.0f, 1.0f);
	if (capabilities)
	{
	    isLeftHand = capabilities->ControllerCapabilities & ovrControllerCaps_LeftHand;
	    handIndex = isLeftHand ? 0 : 1;

		trackpadRange[0] = capabilities->TrackpadMaxX ? static_cast<float>(capabilities->TrackpadMaxX) : 1.0f;
		trackpadRange[1] = capabilities->TrackpadMaxY ? static_cast<float>(capabilities->TrackpadMaxY) : 1.0f;
	}

	mGHInputState.handleGamepadActive(GHGoGamepadIndex, true, GHInputStructs::Gamepad::GPT_VRTOUCH);

	GHInputStructs::Gamepad& ghGamepad = mGHInputState.getGamepadForUpdate(GHGoGamepadIndex);

	GHInputStructs::Poseable& ghHandPose = ghGamepad.mPoseables[handIndex];
	ghHandPose.mActive = true;

	const ovrTracking *handTracking = handDevice->getTracking();
	assert(handTracking);
	if (handTracking)
	{
		updatePose(handTracking->HeadPose, ghHandPose);
		// Go controller doesn't include linear velocity. We'll make a rough guess based on position deltas.
		extrapolateVelocity(handTracking->HeadPose.TimeInSeconds + handTracking->HeadPose.PredictionInSeconds, mHandLastFrameData, ghHandPose);
	}

	handCross.setSizes(0.1, 0.1, 0.1);
	handCross.setTransform(ghHandPose.mPosition);

	const ovrInputStateTrackedRemote* trackedRemoteState = handDevice->getTrackedRemoteInputState();
	assert(trackedRemoteState);
	if (trackedRemoteState)
	{
		//Convert absolute coordinates to -1 -> +1 
		GHPoint2 trackpadPos = GHPoint2(trackedRemoteState->TrackpadPosition.x / trackpadRange[0], trackedRemoteState->TrackpadPosition.y / trackpadRange[1]);
		trackpadPos -= 0.5f;
		trackpadPos *= 2.0f;
        mGHInputState.handleGamepadJoystickChange(GHGoGamepadIndex, handIndex, trackpadPos);

        struct ButtonMap
        {
            ovrButton mOvrButton;
            int mGHButton;
        };

        // Button map specifically for Go controller
        const int ghTriggerKeys[] = { GHKeyDef::KEY_GP_LEFTTRIGGER, GHKeyDef::KEY_GP_RIGHTTRIGGER };
        ButtonMap buttonMap[] =
        {
            { ovrButton_A, ghTriggerKeys[handIndex] },
            { ovrButton_Enter, GHKeyDef::KEY_GP_START },
            { ovrButton_Back, GHKeyDef::KEY_GP_BACK }
        };

        for (int i = 0; i < sizeof(buttonMap)/sizeof(buttonMap[0]); ++i)
        {
            float keyState = (trackedRemoteState->Buttons & buttonMap[i].mOvrButton) ? 1.0f : 0.0f;
            mGHInputState.handleGamepadButtonChange(GHGoGamepadIndex, buttonMap[i].mGHButton, keyState);
        }

		// D-pad translation specifically for Go controller
		// Note: on Oculus Go, ovrButton_Enter is what we get for the trackpad click.
		// We are passing this to GHInputState as KEY_GP_START above (maybe a bad mapping -- subject to change)
		// And then additionally here as a d-pad button, one of left, right, up, down, or A. It's up to the application to choose
		// one interpretation or the other, but note that both buttons will be set.
		//
		// Treating the dpad axes as independent here:
		// It's possible for up to two dpad buttons to be pressed (eg, up+left, up+right, down+left, down+right)
		// It's not possible for two buttons on the same axis to be pressed (eg, up+down, left+right)
		// A button is only passed if trackpad is pressed in the center (eg, no directional buttons).
		if (trackedRemoteState->Buttons & ovrButton_Enter)
		{
			float deadZone = 0.3f;
			bool horizontalDpadButtonPressed = convertTrackpadAxisToDpadButtons(GHGoGamepadIndex, trackpadPos[0], deadZone, GHKeyDef::KEY_GP_DPADLEFT, GHKeyDef::KEY_GP_DPADRIGHT);
			bool verticalDpadButtonPressed = convertTrackpadAxisToDpadButtons(GHGoGamepadIndex, trackpadPos[1], deadZone, GHKeyDef::KEY_GP_DPADUP, GHKeyDef::KEY_GP_DPADDOWN);
			
			//Set the A button only if no dpad buttons were pressed (and unset if they were)
			if (horizontalDpadButtonPressed || verticalDpadButtonPressed)
			{
				mGHInputState.handleGamepadButtonChange(GHGoGamepadIndex, GHKeyDef::KEY_GP_A, 0.0f);
			}
			else
			{
				mGHInputState.handleGamepadButtonChange(GHGoGamepadIndex, GHKeyDef::KEY_GP_A, 1.0f);
			}
		}
		else
		{
			//Since we are polling current state rather than handling 
			// events, need to make sure to unset the buttons.
			mGHInputState.handleGamepadButtonChange(GHGoGamepadIndex, GHKeyDef::KEY_GP_A, 0.0f);
			mGHInputState.handleGamepadButtonChange(GHGoGamepadIndex, GHKeyDef::KEY_GP_DPADLEFT, 0.0f);
			mGHInputState.handleGamepadButtonChange(GHGoGamepadIndex, GHKeyDef::KEY_GP_DPADRIGHT, 0.0f);
			mGHInputState.handleGamepadButtonChange(GHGoGamepadIndex, GHKeyDef::KEY_GP_DPADUP, 0.0f);
			mGHInputState.handleGamepadButtonChange(GHGoGamepadIndex, GHKeyDef::KEY_GP_DPADDOWN, 0.0f);
		}

		//We map ovrButton_Enter (the button under the trackpad) to the GH KEY_GP_START.
		// Therefore, let's set TOUCH_GP_START based on whether the trackpad is being touched, since we can detect that too.
		mGHInputState.handleGamepadButtonChange(GHGoGamepadIndex, GHKeyDef::TOUCH_GP_START, trackedRemoteState->TrackpadStatus ? 1.0f : 0.0f);
		
    }
}

bool GHOvrGoInputTranslator::convertTrackpadAxisToDpadButtons(int gamepadIndex, float normalizedAxisValue, float deadZone, GHKeyDef::Enum negativeKey, GHKeyDef::Enum positiveKey)
{
	if (fabs(normalizedAxisValue) > deadZone)
	{
		if (normalizedAxisValue > 0)
		{
			mGHInputState.handleGamepadButtonChange(gamepadIndex, positiveKey, 1.0f);
			mGHInputState.handleGamepadButtonChange(gamepadIndex, negativeKey, 0.0f);
		}
		else
		{
			mGHInputState.handleGamepadButtonChange(gamepadIndex, positiveKey, 0.0f);
			mGHInputState.handleGamepadButtonChange(gamepadIndex, negativeKey, 1.0f);
		}
		return true;
	}
	else
	{
		//make sure to unset both buttons if we are in the dead zone
		mGHInputState.handleGamepadButtonChange(gamepadIndex, positiveKey, 0.0f);
		mGHInputState.handleGamepadButtonChange(gamepadIndex, negativeKey, 0.0f);
		return false;
	}
}

void GHOvrGoInputTranslator::updatePose(const ovrRigidBodyPosef& inPose, GHInputStructs::Poseable& outPose)
{
	GHOvrGoUtil::convertOvrPoseToGHTransform(inPose.Pose, outPose.mPosition);

	GHPoint3 originPos;
	mHMDOrigin.getTranslate(originPos);
	originPos[1] += mFrameState.getFloorOffset();

	GHTransform originRot = mHMDOrigin;
	originRot.setTranslate(GHPoint3(0, 0, 0));
	outPose.mPosition.mult(originRot, outPose.mPosition);

	GHPoint3 pos;
	outPose.mPosition.getTranslate(pos);
	pos *= mFrameState.getOculusToGameScale();
	pos += originPos;
	outPose.mPosition.setTranslate(pos);

	GHOvrGoUtil::convertOvrVecToGH(inPose.AngularAcceleration, outPose.mAngularAcceleration);
	GHOvrGoUtil::convertOvrVecToGH(inPose.AngularVelocity, outPose.mAngularVelocity);
	GHOvrGoUtil::convertOvrVecToGH(inPose.LinearAcceleration, outPose.mLinearAcceleration);
	GHOvrGoUtil::convertOvrVecToGH(inPose.LinearVelocity, outPose.mLinearVelocity);

	originRot.multDir(outPose.mLinearAcceleration, outPose.mLinearAcceleration);
	originRot.multDir(outPose.mLinearVelocity, outPose.mLinearVelocity);
	originRot.multDir(outPose.mAngularAcceleration, outPose.mAngularAcceleration);
    originRot.multDir(outPose.mAngularVelocity, outPose.mAngularVelocity);
}

void GHOvrGoInputTranslator::extrapolateVelocity(double absoluteTime, LastFrameData& inOutLastFrameData, GHInputStructs::Poseable& outPose)
{
	GHPoint3 currentPosition;
	outPose.mPosition.getTranslate(currentPosition);

	if (inOutLastFrameData.mHasEverBeenSet)
	{
		double timeDiff = absoluteTime - inOutLastFrameData.mAbsoluteTime;
		if (timeDiff > .00001)
		{
			outPose.mLinearVelocity = currentPosition;
			outPose.mLinearVelocity -= inOutLastFrameData.mPosition;
			outPose.mLinearVelocity /= timeDiff;
		}
	}

	inOutLastFrameData.mPosition = currentPosition;
	inOutLastFrameData.mAbsoluteTime = absoluteTime;
	inOutLastFrameData.mHasEverBeenSet = true;
}

#endif
