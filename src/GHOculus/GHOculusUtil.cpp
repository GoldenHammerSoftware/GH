#include "GHOculusUtil.h"
#include "GHMath/GHTransform.h"
#include "GHMath/GHQuaternion.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMath/GHFloat.h"

namespace GHOculusUtil
{
	void convertOvrQuatToGH(const ovrQuatf& inQuat, GHPoint4& outQuat)
	{
		outQuat[0] = inQuat.x;
		outQuat[1] = -inQuat.y;
		outQuat[2] = inQuat.z;
		outQuat[3] = inQuat.w;
	}

	void convertOvrPoseToGHTransform(const ovrPosef& inPose, GHTransform& outTransform)
	{
		GHPoint4 ghQuat;
		convertOvrQuatToGH(inPose.Orientation, ghQuat);

		if (GHFloat::isZero(ghQuat))
		{
			outTransform.becomeIdentity();
		}
		else
		{
			ghQuat.normalize();
			GHQuaternion::convertQuaternionToTransform(ghQuat, outTransform);
		}

		outTransform.setTranslate(GHPoint3(-inPose.Position.x, inPose.Position.y, -inPose.Position.z));
	}

	void convertOvrVecToGH(const ovrVector3f& inVec, GHPoint3& outVec)
	{
		outVec[0] = -inVec.x;
		outVec[1] = inVec.y;
		outVec[2] = -inVec.z;
	}

	bool checkOvrResult(const ovrResult& res, const char* tag)
	{
		if (OVR_SUCCESS(res)) {
			return true;
		}
		ovrErrorInfo errorInfo;
		ovr_GetLastErrorInfo(&errorInfo);
		GHDebugMessage::outputString("*** OCULUS FAILURE *** %s %d %s", tag, res, errorInfo.ErrorString);
		return false;
	}

}