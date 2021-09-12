#include "GHOpenVRHMDTransformApplier.h"
#include <openvr.h>
#include "GHOpenVRInputHandler.h"
#include "GHMath/GHTransform.h"
#include "GHOpenVRUtil.h"

GHOpenVRHMDTransformApplier::GHOpenVRHMDTransformApplier(const GHOpenVRInputHandler& inputSource, const GHTransform& hmdOrigin, GHTransform& outTrans)
	: mInputSource(inputSource)
	, mHMDOrigin(hmdOrigin)
	, mOutTrans(outTrans)
{

}

void GHOpenVRHMDTransformApplier::update(void)
{
	const vr::TrackedDevicePose_t& headPose = mInputSource.getCurrentHeadPose();

	if (!headPose.bDeviceIsConnected) {
		return;
	}
	
	if (!headPose.bPoseIsValid) {
		return;
	}


	GHOpenVRUtil::convertToGHTransform(headPose.mDeviceToAbsoluteTracking, mOutTrans);

	mOutTrans.mult(mHMDOrigin, mOutTrans);
}
