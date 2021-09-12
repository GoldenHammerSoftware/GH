#include "GHOculusHMDTransformApplier.h"
#include "GHMath/GHTransform.h"
#include "GHOculusDX11HMDRenderDevice.h"
#include "GHMath/GHQuaternion.h"

GHOculusHMDTransformApplier::GHOculusHMDTransformApplier(GHOculusDX11HMDRenderDevice& hmdDevice, const GHTransform& hmdOrigin, GHTransform& outTrans)
	: mHMDOrigin(hmdOrigin)
	, mOutTrans(outTrans)
	, mHMDDevice(hmdDevice)
{
}

void GHOculusHMDTransformApplier::update(void)
{
	if (!mHMDDevice.isActive())
	{
		return;
	}

	const ovrTrackingState& trackingState = mHMDDevice.getTrackingState();
	const ovrVector3f& ovrPos = trackingState.HeadPose.ThePose.Position;
	GHPoint3 ghOvrPos(-ovrPos.x, ovrPos.y, -ovrPos.z);
	ghOvrPos *= mHMDDevice.getOculusToGameScale();
	GHPoint3 rotatedGHOvrPos;
	mHMDOrigin.multDir(ghOvrPos, rotatedGHOvrPos);

	GHPoint3 rootPos;
	mHMDOrigin.getTranslate(rootPos);
	rotatedGHOvrPos += rootPos;

	const ovrQuatf headQuat = trackingState.HeadPose.ThePose.Orientation;
	GHPoint4 ghQuat(headQuat.x, -headQuat.y, headQuat.z, headQuat.w);
	GHTransform hmdRotate;
	GHQuaternion::convertQuaternionToTransform(ghQuat, hmdRotate);

	GHTransform originRotate = mHMDOrigin;
	originRotate.setTranslate(GHPoint3(0, 0, 0));

	mOutTrans = hmdRotate;
	mOutTrans.mult(originRotate, mOutTrans);
	mOutTrans.setTranslate(rotatedGHOvrPos);
}
