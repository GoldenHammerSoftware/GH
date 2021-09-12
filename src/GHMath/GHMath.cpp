// Copyright Golden Hammer Software
#include "GHMath.h"
#include "GHPi.h"
#include <vector>

void GHMath::cross(const GHPoint3& first, const GHPoint3& second, GHPoint3& ret)
{
	ret[0] = first[1] * second[2] - first[2] * second[1];
	ret[1] = first[2] * second[0] - first[0] * second[2];
	ret[2] = first[0] * second[1] - first[1] * second[0];
}

void GHMath::calcNormal(const GHPoint3& p1, const GHPoint3& p2, const GHPoint3& p3, GHPoint3& ret)
{
	GHPoint3 v1,v2;
	p2.minus(p1, v1);
	p3.minus(p1, v2);
	GHMath::cross(v1, v2, ret);
	ret.normalize();
}

void GHMath::calcReflectionVector(const GHPoint3& incident, const GHPoint3& normal, GHPoint3& ret)
{
    // r = i - (2 * n * dot(i, n))
    
    float dotIN = incident * normal;
    GHPoint3 normMod(normal);
    normMod *= 2.0;
    normMod *= dotIN;
    ret = incident;
    ret -= normMod;
}

float GHMath::deg2Rad(float angleInDegrees)
{
	return angleInDegrees * (GHPI/180);
}

// used for invert, taken from http://www.flipcode.com/documents/matrfaq.html#Q17
// calcs the determinant of a 3x3 matrix
float GHMath::calcDeterminant3x3(GHPoint<float, 9>& mat)
{
    float det;
    
    det = mat[0] * ( mat[4]*mat[8] - mat[7]*mat[5] )
        - mat[1] * ( mat[3]*mat[8] - mat[6]*mat[5] )
        + mat[2] * ( mat[3]*mat[7] - mat[6]*mat[4] );
    
    return det;
}

float GHMath::distRadians(float rad1, float rad2)
{
	// first remove multiple rotations around the circle.
	rad1 = (float)fmod(rad1, GHPI*2.0f);
	if (rad1 < 0) rad1 += (GHPI*2.0f);
	rad2 = (float)fmod(rad2, GHPI*2.0f);
	if (rad2 < 0) rad2 += (GHPI*2.0f);

	// get the shortest distance using multiple methods.
	float regDist = (float)fabs(rad1 - rad2);
	if (rad1 < rad2) {
		rad1 += GHPI*2.0f;
	}
	else if (rad2 < rad1) {
		rad2 += GHPI*2.0f;
	}
	float modDist = (float)fabs(rad1 - rad2);
	if (modDist < regDist) {
		return modDist;
	}
	return regDist;
}

void GHMath::transposeMatrix4(const float* source, float* dest)
{
	/*
	0 1 2 3
	4 5 6 7
	8 9 10 11
	12 13 14 15
	*/

	dest[0] = source[0];
	dest[1] = source[4];
	dest[2] = source[8];
	dest[3] = source[12];

	dest[4] = source[1];
	dest[5] = source[5];
	dest[6] = source[9];
	dest[7] = source[13];

	dest[8] = source[2];
	dest[9] = source[6];
	dest[10] = source[10];
	dest[11] = source[14];

	dest[12] = source[3];
	dest[13] = source[7];
	dest[14] = source[11];
	dest[15] = source[15];
}

void GHMath::transposeMatrix3(const float* source, float* dest)
{
	/*
	0 1 2 
	3 4 5 
	6 7 8
	*/
	dest[0] = source[0];
	dest[1] = source[3];
	dest[2] = source[6];

	dest[3] = source[1];
	dest[4] = source[4];
	dest[5] = source[7];

	dest[6] = source[2];
	dest[7] = source[5];
	dest[8] = source[8];
}
