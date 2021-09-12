// Copyright Golden Hammer Software
#pragma once

#include "GHPoint.h"

class GHTransform;

namespace GHQuaternion
{
    static const GHPoint4 IDENTITY(0,0,0,1);
    
    void slerp(const GHPoint4& src, const GHPoint4& dst, float interpVal, GHPoint4& ret);
    
    void convertQuaternionToTransform(const GHPoint4& quat, GHTransform& outTransform);
    void convertQuaternionToTransform(const GHPoint4& quat, GHPoint16& outTransform);
	void convertTransformToQuaternion(const GHTransform& xForm, GHPoint4& outQuat);
    void convertTransformToQuaternion(const GHPoint16& xForm, GHPoint4& outQuat);

	void multiply(const GHPoint4& lhs, const GHPoint4& rhs, GHPoint4& outQuat);
}
