#include "GHHMDRecenterController.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHMath/GHTransform.h"
#include "GHInputState.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHKeyDef.h"
#include "GHMath/GHPi.h"
#include "GHMath/GHMath.h"
#include "GHUtils/GHEventMgr.h"
#include "GHHMDIdentifiers.h"
#include "GHUtils/GHMessage.h"

GHHMDRecenterController::GHHMDRecenterController(const GHTimeVal& timeVal,
												GHPropertyContainer& globalProps,
												const GHIdentifier& hmdCenterId,
												const GHIdentifier& hmdCenterDefaultId,
												const GHIdentifier& optionsSaveMessageId,
												GHTransform& cameraCenter,
												const GHInputState& inputState,
												GHEventMgr& eventMgr,
												unsigned int gamepadIndex)
	: mConfirmationHandler(eventMgr, globalProps, hmdCenterId, hmdCenterDefaultId, optionsSaveMessageId, cameraCenter)
	, mTimeVal(timeVal)
	, mGlobalProps(globalProps)
	, mCameraCenter(cameraCenter)
	, mInputState(inputState)
	, mGamepadIndex(gamepadIndex)
{

}

void GHHMDRecenterController::onActivate(void)
{
	mConfirmationHandler.onActivate();
}

void GHHMDRecenterController::onDeactivate(void)
{
	mConfirmationHandler.onDeactivate();
}

void GHHMDRecenterController::update(void)
{
	implementationWithHMDPos();
}

void GHHMDRecenterController::implementaitonWithJoysticks(void)
{
	float translateSpeed = 1.f;
	float rotateSpeed = 1.f;

	GHPoint3 translate;
	mCameraCenter.getTranslate(translate);

	auto& leftJoy = mInputState.getGamepad(mGamepadIndex).mJoySticks[GHInputStructs::Gamepad::LEFT];
	auto& rightJoy = mInputState.getGamepad(mGamepadIndex).mJoySticks[GHInputStructs::Gamepad::RIGHT];

	GHPoint3 moveDir(0, 0, 0);
	moveDir[0] -= leftJoy[0];
	moveDir[2] += leftJoy[1];
	moveDir.normalize();
	moveDir *= translateSpeed * mTimeVal.getTimePassed();

	GHPoint3 rotateAxis(0, 1, 0);
	float rotateAngle = 0;
	rotateAngle += rotateSpeed * mTimeVal.getTimePassed() * rightJoy[0];

	GHTransform rotateTrans;
	rotateTrans.becomeAxisAngleRotation(rotateAxis, rotateAngle);

	GHTransform currRot = mCameraCenter;
	currRot.setTranslate(GHPoint3(0, 0, 0));
	currRot.mult(rotateTrans, mCameraCenter);

	mCameraCenter.multDir(moveDir, moveDir);
	translate += moveDir;
	mCameraCenter.setTranslate(translate);
}

void GHHMDRecenterController::implementationWithHMDPos(void)
{
	//the keys we press to change the HMD origin in this mode
	if (mInputState.checkGamepadKeyChange(mGamepadIndex, GHKeyDef::KEY_GP_A, true)
		|| mInputState.checkGamepadKeyChange(mGamepadIndex, GHKeyDef::KEY_GP_X, true))
	{
		GHPoint3 originalHMDOriginTranslate;
		mCameraCenter.getTranslate(originalHMDOriginTranslate);

		GHPoint3 originalPoseableTranslate;
		mInputState.getPoseable(0).mPosition.getTranslate(originalPoseableTranslate);

		GHTransform absoluteTransform;
		//the poseable is w.r.t the hmd rotation
		GHTransform originalHMDOriginInv = mCameraCenter;
		originalHMDOriginInv.setTranslate(GHPoint3(0, 0, 0));
		originalHMDOriginInv.invert();

		GHTransform originalPoseable = mInputState.getPoseable(0).mPosition;
		originalPoseable.setTranslate(GHPoint3(0, 0, 0));

		originalPoseable.mult(originalHMDOriginInv, absoluteTransform);
		
		const GHPoint3 absForward(0, 0, -1);
		GHPoint3 currForward;
		absoluteTransform.multDir(absForward, currForward);
		currForward[1] = 0; //zero out the y-component so we only get the angle between the vectors on the XZ plane
		currForward.normalize();

		GHPoint3 cross;
		GHMath::cross(absForward, currForward, cross);
		GHPoint3 up(0, 1, 0);
		float sign = GHMath::sign(cross*up);

		mCameraCenter.becomeYRotation(-sign*std::acos(absForward*currForward));

		//The platform->GH system will take the system's poseable position (which we can't change) and add it to our HMD offset
		// We want our new HMD offset to be placed such that the final poseable is at the origin (plus our offset)
		// this means we find the absolute poseable position unmodified by the old origin and then modify it by the new origin
		GHPoint3 poseablePosWRTOrigin = originalPoseableTranslate;
		poseablePosWRTOrigin -= originalHMDOriginTranslate;
		originalHMDOriginInv.mult(poseablePosWRTOrigin, poseablePosWRTOrigin);

		GHPoint3 translate = poseablePosWRTOrigin;
		mCameraCenter.multDir(translate, translate);
		translate *= -1.f; //what we actually want here is the origin's offset from the head position rather than the head position's offset from the origin
		translate += GHPoint3(-1.0, 0.0, -1.0); //temporary offset. Todo: data-drive from game (this offset is game-specific)

		translate[1] = originalHMDOriginTranslate[1]; //again, we are sticking to the XZ plane. Plus the original offset includes the player height

		mCameraCenter.setTranslate(translate);
	}
}

GHHMDRecenterController::ConfirmationHandler::ConfirmationHandler(	GHEventMgr& eventMgr, GHPropertyContainer& props,
																	const GHIdentifier& hmdCenterID,
																	const GHIdentifier& hmdCenterDefaultID,
																	const GHIdentifier& optionsSaveMessageID,
																	GHTransform& cameraCenter)
	: mEventMgr(eventMgr)
	, mProps(props)
	, mCameraCenter(cameraCenter)
	, mHMDCenterID(hmdCenterID)
	, mHMDCenterDefaultID(hmdCenterDefaultID)
	, mOptionsSaveMessageID(optionsSaveMessageID)
{
	mEventMgr.registerListener(GHHMDIdentifiers::M_HMDCENTEROFFSETRESETTODEFAULT, *this);
}

GHHMDRecenterController::ConfirmationHandler::~ConfirmationHandler()
{
	mEventMgr.unregisterListener(GHHMDIdentifiers::M_HMDCENTEROFFSETRESETTODEFAULT, *this);
}

void GHHMDRecenterController::ConfirmationHandler::onActivate(void)
{
	mBackupTransform = mCameraCenter;
	mEventMgr.registerListener(GHHMDIdentifiers::M_HMDCENTEROFFSETACCEPT, *this);
	mEventMgr.registerListener(GHHMDIdentifiers::M_HMDCENTEROFFSETCANCEL, *this);

}

void GHHMDRecenterController::ConfirmationHandler::onDeactivate(void)
{
	mEventMgr.unregisterListener(GHHMDIdentifiers::M_HMDCENTEROFFSETCANCEL, *this);
	mEventMgr.unregisterListener(GHHMDIdentifiers::M_HMDCENTEROFFSETACCEPT, *this);
}

void GHHMDRecenterController::ConfirmationHandler::handleMessage(const GHMessage& message)
{
	if (message.getType() == GHHMDIdentifiers::M_HMDCENTEROFFSETACCEPT)
	{
		setHMDCenterProperty(mCameraCenter.getMatrix());
		mEventMgr.handleMessage(GHMessage(mOptionsSaveMessageID));
	}
	else if (message.getType() == GHHMDIdentifiers::M_HMDCENTEROFFSETCANCEL)
	{
		mCameraCenter = mBackupTransform;
	}
	else if (message.getType() == GHHMDIdentifiers::M_HMDCENTEROFFSETRESETTODEFAULT)
	{
		mCameraCenter.getMatrix() = getHMDCenterDefault();
	}
}

void GHHMDRecenterController::ConfirmationHandler::setHMDCenterProperty(const GHPoint16& matrix)
{
	getOrSetMatrixProperty(mHMDCenterID, matrix, true);
}

const GHPoint16& GHHMDRecenterController::ConfirmationHandler::getHMDCenterDefault(void)
{
	return getOrSetMatrixProperty(mHMDCenterDefaultID, GHTransform::IDENTITY.getMatrix(), false);
} 

GHPoint16& GHHMDRecenterController::ConfirmationHandler::getOrSetMatrixProperty(const GHIdentifier& propID, const GHPoint16& newMatrix, bool overrideExistingValue)
{
	GHPoint<float, 16>* matrix = mProps.getProperty(propID);
	if (!matrix)
	{
		matrix = new GHPoint<float, 16>;
		*matrix = newMatrix;
		mProps.setProperty(propID, GHProperty(matrix, new GHRefCountedType<GHPoint<float, 16> >(matrix)));
	}
	else if (overrideExistingValue)
	{
		*matrix = newMatrix;
	}
	return *matrix;
}
