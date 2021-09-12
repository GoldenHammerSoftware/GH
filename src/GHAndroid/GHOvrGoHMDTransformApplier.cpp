#ifdef GH_OVR_GO

#include "GHOvrGoHMDTransformApplier.h"
#include "GHMath/GHTransform.h"
#include "GHMath/GHQuaternion.h"
#include "VrApi.h"
#include "VrApi_Helpers.h"
#include "VrApi_SystemUtils.h"
#include "GHOvrGoFrameState.h"

GHOvrGoHMDTransformApplier::GHOvrGoHMDTransformApplier(const GHOvrGoFrameState& frameState, const GHTransform& hmdOrigin,
	GHTransform& outTrans)
: mFrameState(frameState)
, mOutTrans(outTrans)
, mHMDOrigin(hmdOrigin)
{
}

void GHOvrGoHMDTransformApplier::update(void)
{
	if (!mFrameState.mIsTrackingReady)
	{
		return;
	}

	const ovrRigidBodyPosef& headRigidBodyPose = mFrameState.mPredictedTracking.HeadPose;
	const ovrPosef& headPose = headRigidBodyPose.Pose;
	const ovrVector3f& ovrPos = headPose.Position;

	GHPoint3 ghOvrPos(-ovrPos.x, ovrPos.y, -ovrPos.z);
	ghOvrPos *= mFrameState.getOculusToGameScale();
	GHPoint3 rotatedGHOvrPos;
	mHMDOrigin.multDir(ghOvrPos, rotatedGHOvrPos);

	GHPoint3 rootPos;
	mHMDOrigin.getTranslate(rootPos);
	rotatedGHOvrPos += rootPos;
	// test hack? move the camera up some.
	rotatedGHOvrPos[1] += mFrameState.getFloorOffset();

	const ovrQuatf headQuat = headPose.Orientation;
	GHPoint4 ghQuat(headQuat.x, -headQuat.y, headQuat.z, headQuat.w);
	GHTransform hmdRotate;
	GHQuaternion::convertQuaternionToTransform(ghQuat, hmdRotate);

	GHTransform originRotate = mHMDOrigin;
	originRotate.setTranslate(GHPoint3(0, 0, 0));

	mOutTrans = hmdRotate;
	mOutTrans.mult(originRotate, mOutTrans);
	mOutTrans.setTranslate(rotatedGHOvrPos);
}

#endif
