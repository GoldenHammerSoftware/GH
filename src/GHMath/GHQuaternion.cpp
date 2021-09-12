// Copyright Golden Hammer Software
#include "GHQuaternion.h"
#include "GHMath.h"
#include "GHFloat.h"
#include "GHPi.h"
#include "GHTransform.h"

namespace GHQuaternion 
{
    void slerp(const GHPoint4& src, const GHPoint4& target, float interpVal, GHPoint4& ret)
    {
        GHPoint4 other = target;
        float cosAngle = src[0]*target[0] + src[1]*target[1] + src[2]*target[2] + src[3]*target[3];

		cosAngle = GHMath::clamp(-1.f, 1.f, cosAngle);
        
        if(cosAngle < 0)
        {
            cosAngle = -cosAngle;
            other *= -1.f;
        }
        
        //handle degenerate case where we are interpolating between two of the same quaternion
        if(GHFloat::isEqual(cosAngle, 1.f))
        {
            ret = src;
            return;
        }
        
        float angle = (float)::acos(cosAngle);
        
        float divSinAngle = 1.f / (float)::sin(angle);
        ret = src;
        ret *= (float)::sin((1.f - interpVal)*angle);
        
        other *= (float)::sin(interpVal*angle);
        ret += other;
        
        ret *= divSinAngle;
        
        ret.normalize();
    }
    
    //Conversion Methods taken from the book Game Physics, by David H Eberly. Published by Morgan Kaufmann Publishers in 2004.
	// Both formulas can be found on page 537, in Chapter 10: Quaternions.
	
	void convertQuaternionToTransform(const GHPoint4& quat, GHTransform& outTransform)
	{
        convertQuaternionToTransform(quat, outTransform.getMatrix());
        outTransform.incrementVersion();
	}

    void convertQuaternionToTransform(const GHPoint4& quat, GHPoint16& outTransform)
    {
		assert(GHFloat::isEqual(quat.lenSqr(), 1.f));
        
		const float& x = quat[0];
		const float& y = quat[1];
		const float& z = quat[2];
		const float& w = quat[3];
        
		// local convention: n2 = n squared
		float x2 = x*x;
		float y2 = y*y;
		float z2 = z*z;
        
		outTransform[0] = 1.f - 2.f*(z2 + y2);
        outTransform[1] = 2.f*(x*y - w*z);
        outTransform[2] = 2.f*(z*x + w*y);
        outTransform[3] = 0;
        
        outTransform[4] = 2.f*(x*y + w*z);
        outTransform[5] = 1.f - 2.f*(x2 + z2);
        outTransform[6] = 2.f*(y*z - w*x);
        outTransform[7] = 0;
        
        outTransform[8] = 2.f*(z*x - w*y);
        outTransform[9] = 2.f*(y*z + w*x);
        outTransform[10] = 1.f - 2.f*(x2 + y2);
        outTransform[11] = 0;
        
        outTransform[12] = 0;
        outTransform[13] = 0;
        outTransform[14] = 0;
        outTransform[15] = 1;
    }
	
	void convertTransformToQuaternion(const GHTransform& transform, GHPoint4& outQuat)
	{
		const GHPoint<float, 16>& xForm = transform.getMatrix();
        convertTransformToQuaternion(xForm, outQuat);
	}

    void convertTransformToQuaternion(const GHPoint16& xForm, GHPoint4& outQuat)
    {
		float trace = xForm[0] + xForm[5] + xForm[10];
		if(trace < 0)
		{
			int i = 0, j = 1, k = 2;
			if(xForm[5] > xForm[0])	{ i = 1, j = 2, k = 0; }
			if(xForm[10] > xForm[i*4+i]) { i = 2, j = 0, k = 1; }
			
			float r = (float)::sqrt( xForm[i*4+i] - xForm[j*4+j] - xForm[k*4+k] + 1.f);
			float s = .5f / r;
			outQuat[i] = .5f * r;
			outQuat[j] = ((xForm[i*4+j] + xForm[j*4+i])*s);
			outQuat[k] = ((xForm[k*4+i] + xForm[i*4+k])*s);
			outQuat[3] = ((xForm[k*4+j] - xForm[j*4+k])*s);
		}
		else
		{
			float r = (float)::sqrt(trace + 1);
			float s = .5f/r;
			outQuat[0] = (xForm[9] - xForm[6])*s;
			outQuat[1] = (xForm[2] - xForm[8])*s;
			outQuat[2] = (xForm[4] - xForm[1])*s;
			outQuat[3] = .5f * r;
		}
    }

	//from https://www.mathworks.com/help/aeroblks/quaternionmultiplication.html?requestedDomain=www.mathworks.com
	void multiply(const GHPoint4& lhs, const GHPoint4& rhs, GHPoint4& outQuat)
	{
		GHPoint4 temp;	
		temp[0] = lhs[0]*rhs[0] - lhs[1]*rhs[1] - lhs[2]*rhs[2] - lhs[3]*rhs[3];
		temp[1] = lhs[0]*rhs[1] + lhs[1]*rhs[0] - lhs[2]*rhs[3] + lhs[3]*rhs[2];
		temp[2] = lhs[0]*rhs[2] + lhs[1]*rhs[3] + lhs[2]*rhs[0] - lhs[3]*rhs[1];
		temp[3] = lhs[0]*rhs[3] - lhs[1]*rhs[2] + lhs[2]*rhs[1] + lhs[2]*rhs[0];
		outQuat = temp;
	}

}
