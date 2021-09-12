#pragma once

#include "OVR_CAPI.h"
#include "GHMath/GHPoint.h"
#include <vector>

class GHTransform;
class GHXMLSerializer;

namespace GHOculusUtil
{
	void convertOvrQuatToGH(const ovrQuatf& inQuat, GHPoint4& outQuat);

	void convertOvrPoseToGHTransform(const ovrPosef& inPose, GHTransform& outTransform);

	void convertOvrVecToGH(const ovrVector3f& inVec, GHPoint3& outVec);

	bool checkOvrResult(const ovrResult& res, const char* tag);
}
