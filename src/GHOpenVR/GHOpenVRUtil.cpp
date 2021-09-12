#include "GHOpenVRUtil.h"
#include "GHMath/GHTransform.h"
#include "GHMath/GHQuaternion.h"

namespace GHOpenVRUtil
{
	//---------------------------------------
	// Derivation for matrix conversion:
	//	The generic conversion of a matrix transformation M from one space to another,
	//	where C is the representation of the source space in the destination space
	//	(ie, conversion matrix used for vectors) is  C * M * inverseC
	// 
	// From the OpenVR header comment:
	//		right-handed system
	//		+y is up
	//		+x is to the right
	//		-z is forward
	// GH space is the same, except X and Z point in the opposite direction. Therefore, the
	// matrix converting from one to the other is
	//		| -1  0  0  0 |
	//		|  0  1  0  0 |
	//      |  0  0 -1  0 |
	//		|  0  0  0  1 |
	// (This matrix is is also an involutory matrix, ie its inverse is itself, and the same 
	// conversion will also convert from GH space to OpenVR space).
	// 
	//  Additionally, GH matrices are row-major whereas OpenVR matrices are column-major, so we take the transpose.
	// 
	// The following seemingly obtuse pattern of negation was derived from
	// solving for C * transposeM * inverseC with the above conversion matrix.

	void convertToGHTransform(const vr::HmdMatrix44_t& inTrans, GHTransform& outTrans)
	{
		outTrans[0 * 4 + 0] = inTrans.m[0][0];
		outTrans[0 * 4 + 1] = -inTrans.m[1][0];
		outTrans[0 * 4 + 2] = inTrans.m[2][0];
		outTrans[0 * 4 + 3] = -inTrans.m[3][0];
		
		outTrans[1 * 4 + 0] = -inTrans.m[0][1];
		outTrans[1 * 4 + 1] = inTrans.m[1][1];
		outTrans[1 * 4 + 2] = -inTrans.m[2][1];
		outTrans[1 * 4 + 3] = inTrans.m[3][1];

		outTrans[2 * 4 + 0] = inTrans.m[0][2];
		outTrans[2 * 4 + 1] = -inTrans.m[1][2];
		outTrans[2 * 4 + 2] = inTrans.m[2][2];
		outTrans[2 * 4 + 3] = -inTrans.m[3][2];

		outTrans[3 * 4 + 0] = -inTrans.m[0][3];
		outTrans[3 * 4 + 1] = inTrans.m[1][3];
		outTrans[3 * 4 + 2] = -inTrans.m[2][3];
		outTrans[3 * 4 + 3] = inTrans.m[3][3];
	}

	void convertToGHTransform(const vr::VRBoneTransform_t& inTrans, GHTransform& outTrans)
	{
		convertToGHTransform(inTrans.orientation, outTrans);
		GHPoint3 position;
		convertToGHPoint3(inTrans.position, position);
		outTrans.setTranslate(position);
	}

	void convertToGHTransform(const vr::HmdQuaternionf_t& inQuat, GHTransform& outTrans)
	{
		GHPoint4 ghQuat;
		convertToGHQuaternion(inQuat, ghQuat);
		GHQuaternion::convertQuaternionToTransform(ghQuat, outTrans);
	}

	void convertToGHQuaternion(const vr::HmdQuaternionf_t& inQuat, GHPoint4& outQuat)
	{
		outQuat[0] = inQuat.x;
		outQuat[1] = -inQuat.y;
		outQuat[2] = inQuat.z;
		outQuat[3] = inQuat.w;
	}

	void convertToGHTransform(const vr::HmdMatrix34_t& inTrans, GHTransform& outTrans)
	{
		outTrans[0 * 4 + 0] = inTrans.m[0][0];
		outTrans[0 * 4 + 1] = -inTrans.m[1][0];
		outTrans[0 * 4 + 2] = inTrans.m[2][0];
		outTrans[0 * 4 + 3] = 0;// -inTrans.m[3][0];

		outTrans[1 * 4 + 0] = -inTrans.m[0][1];
		outTrans[1 * 4 + 1] = inTrans.m[1][1];
		outTrans[1 * 4 + 2] = -inTrans.m[2][1];
		outTrans[1 * 4 + 3] = 0;// inTrans.m[3][1];

		outTrans[2 * 4 + 0] = inTrans.m[0][2];
		outTrans[2 * 4 + 1] = -inTrans.m[1][2];
		outTrans[2 * 4 + 2] = inTrans.m[2][2];
		outTrans[2 * 4 + 3] = 0;// -inTrans.m[3][2];

		outTrans[3 * 4 + 0] = -inTrans.m[0][3];
		outTrans[3 * 4 + 1] = inTrans.m[1][3];
		outTrans[3 * 4 + 2] = -inTrans.m[2][3];
		outTrans[3 * 4 + 3] = 1;// inTrans.m[3][3];
	}

	void convertToGHPoint3(const vr::HmdVector3_t& inVec, GHPoint3& outVec)
	{
		outVec[0] = -inVec.v[0];
		outVec[1] = inVec.v[1];
		outVec[2] = -inVec.v[2];
	}

	void convertToGHPoint3(const vr::HmdVector4_t& inVec, GHPoint3& outVec)
	{
		outVec[0] = -inVec.v[0];
		outVec[1] = inVec.v[1];
		outVec[2] = -inVec.v[2];
	}
}