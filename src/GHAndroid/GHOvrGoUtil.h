#pragma once

#ifdef GH_OVR_GO

#include "VrApi.h"
#include "VrApi_Helpers.h"
#include "VrApi_SystemUtils.h"

#include "GHMath/GHPoint.h"

class GHTransform;

// This is very similar almost copy and paste code from GHOculusUtil
namespace GHOvrGoUtil
{
	void convertOvrQuatToGH(const ovrQuatf& inQuat, GHPoint4& outQuat);

	void convertOvrPoseToGHTransform(const ovrPosef& inPose, GHTransform& outTransform);

	void convertOvrVecToGH(const ovrVector3f& inVec, GHPoint3& outVec);
}

#endif

