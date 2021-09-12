#include "GHOvrGoAvatarPoser.h"
#include "VrApi_Input.h"
#include "GHOvrGoInputState.h"
#include "GHOvrGoFrameState.h"

void convertPoseToAvatarTransform(const ovrPosef& inPose, ovrAvatarTransform& outTrans)
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

GHOvrGoAvatarPoser::GHOvrGoAvatarPoser(const GHOvrGoFrameState& frameState, const GHOvrGoInputState& inputState)
	: mFrameState(frameState)
	, mInputState(inputState)
{

}

void GHOvrGoAvatarPoser::poseAvatar(ovrAvatar* avatar)
{
    if (!avatar)
    {
        return;
    }

	updateBody(avatar);

	//todo: microphone input
	//ovrAvatarPose_UpdateVoiceVisualization(sAvatar, (uint32_t)sampleCount, samples); 

	updateHands(avatar);

	double deltaSeconds = mFrameState.mPredictedDisplayTime;
	deltaSeconds -= mLastUpdateTime;
	mLastUpdateTime = mFrameState.mPredictedDisplayTime;

	ovrAvatarPose_Finalize(avatar, deltaSeconds);
}

void GHOvrGoAvatarPoser::updateBody(ovrAvatar* avatar)
{
	//It turns out there is no GI_HEADSET on Go -- leaving this in in case we need it for Gear VR
	//const GHOvrGoInputState::OvrGoInputDevice* headDevice = mInputState.getDevice(GHOvrGoInputState::GI_HEADSET);
	//if (!headDevice || !headDevice->isActive())
	//{
	//	return;
	//}
//
//
	//if (!headDevice->getTracking())
	//{
	//	assert(false && "Error in GHOvrGoAvatarPoser: If a head exists, we expect it to have tracking.");
	//	return;
	//}

	ovrAvatarTransform avatarTransform;
	//convertPoseToAvatarTransform(headDevice->getTracking()->HeadPose.Pose, avatarTransform);
	convertPoseToAvatarTransform(mFrameState.mPredictedTracking.HeadPose.Pose, avatarTransform);
	ovrAvatarPose_UpdateBody(avatar, avatarTransform);
}

void GHOvrGoAvatarPoser::updateHands(ovrAvatar* avatar)
{
	const GHOvrGoInputState::OvrGoInputDevice* handDevice = mInputState.getDevice(GHOvrGoInputState::GI_CONTROLLER);
	if (!handDevice)
	{
		return;
	}

	if (!handDevice->getTracking() || !handDevice->getTrackedRemoteCapabilities() || !handDevice->getTrackedRemoteInputState())
	{
		assert(false && "Error in GHOvrGoAvatarPoser: If a controller exists, we expect it to be a tracked controller.");
		return;
	}

	//inputs
	const ovrTracking& trackingState = *handDevice->getTracking();
	const ovrInputStateTrackedRemote& remoteInputState = *handDevice->getTrackedRemoteInputState();
	const ovrInputTrackedRemoteCapabilities& capabilities = *handDevice->getTrackedRemoteCapabilities();
	const bool isActive = handDevice->isActive();

	//locals
	ovrAvatarTransform controllerTransform;
	ovrAvatarHandInputState avatarInputState = { 0 };
	ovrAvatarHandInputState dummyInputState = { 0 };

	if (isActive)
	{
		convertPoseToAvatarTransform(trackingState.HeadPose.Pose, avatarInputState.transform);
		avatarInputState.buttonMask = 0;
		avatarInputState.touchMask = remoteInputState.TrackpadStatus;

		// Normalize these from 0 to 1
		avatarInputState.joystickX = remoteInputState.TrackpadPosition.x / capabilities.TrackpadMaxX;
		avatarInputState.joystickY = remoteInputState.TrackpadPosition.y / capabilities.TrackpadMaxY;
		avatarInputState.indexTrigger = 0.f;
		avatarInputState.handTrigger = 0.f;
		avatarInputState.isActive = true;

		if (remoteInputState.Buttons & ovrButton_A)		avatarInputState.buttonMask |= ovrAvatarButton_One;
		if (remoteInputState.Buttons & ovrButton_Enter)	avatarInputState.buttonMask |= ovrAvatarButton_Two;
		if (remoteInputState.Buttons & ovrButton_Back)	avatarInputState.buttonMask |= ovrAvatarButton_Three;
	}

	const bool isLeft = capabilities.ControllerCapabilities & ovrControllerCaps_LeftHand;
	auto& leftInputState = isLeft ? avatarInputState : dummyInputState;
	auto& rightInputState = !isLeft ? avatarInputState : dummyInputState;

	ovrAvatarPose_Update3DofHands( avatar,
									&leftInputState,
									&rightInputState,
									ovrAvatarControllerType_Go );
}

GHOvrGoAvatarPoserFactory::GHOvrGoAvatarPoserFactory(const GHOvrGoFrameState& frameState, const GHOvrGoInputState& inputState)
	: mFrameState(frameState)
	, mInputState(inputState)
{

}

GHOculusAvatarPlatformPoser* GHOvrGoAvatarPoserFactory::createAvatarPoser(void)
{
	return new GHOvrGoAvatarPoser(mFrameState, mInputState);
}
