#pragma once
#include <openvr.h>
#include "GHMath/GHPoint.h"

class GHTransform;

namespace GHOpenVRUtil
{
	void convertToGHTransform(const vr::HmdMatrix44_t& inTrans, GHTransform& outTrans);
	void convertToGHTransform(const vr::HmdMatrix34_t& inTrans, GHTransform& outTrans);
	void convertToGHTransform(const vr::VRBoneTransform_t& inTrans, GHTransform& outTrans);
	void convertToGHTransform(const vr::HmdQuaternionf_t& inQuat, GHTransform& outTrans);
	void convertToGHQuaternion(const vr::HmdQuaternionf_t& inQuat, GHPoint4& outQuat);
	void convertToGHPoint3(const vr::HmdVector3_t& inVec, GHPoint3& outVec);
	void convertToGHPoint3(const vr::HmdVector4_t& inVec, GHPoint3& outVec);
}
