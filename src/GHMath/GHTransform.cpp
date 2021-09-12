// Copyright Golden Hammer Software
#include "GHMathInclude.h"
#include "GHMathDebugPrint.h"
#include "GHTransform.h"
#include "GHFloat.h"
#include "GHMath.h"
#include <stddef.h>
#include <cmath>
#include "GHPi.h"
#include "GHQuaternion.h"

GHTransform GHTransform::IDENTITY(1,0,0,0,
                                  0,1,0,0,
                                  0,0,1,0,
                                  0,0,0,1);

GHTransform::GHTransform(void)
: mVersion(INITIAL_VERSION)
{
}

GHTransform::GHTransform(float f1, float f2, float f3, float f4,
                         float f5, float f6, float f7, float f8, 
                         float f9, float f10, float f11, float f12,
                         float f13, float f14, float f15, float f16)
: mVersion(INITIAL_VERSION)
{
	mMatrix[0] = f1;
	mMatrix[1] = f2;
	mMatrix[2] = f3;
	mMatrix[3] = f4;
	mMatrix[4] = f5;
	mMatrix[5] = f6;
	mMatrix[6] = f7;
	mMatrix[7] = f8;
	mMatrix[8] = f9;
	mMatrix[9] = f10;
	mMatrix[10] = f11;
	mMatrix[11] = f12;
	mMatrix[12] = f13;
	mMatrix[13] = f14;
	mMatrix[14] = f15;
	mMatrix[15] = f16;
}

GHTransform::GHTransform(const GHPoint16& matrix)
	: mVersion(INITIAL_VERSION)
{
	mMatrix = matrix;
}

void GHTransform::mult(const GHTransform& other, GHTransform& ret) const
{
	mult(getMatrix(), other.getMatrix(), ret.getMatrix());

    ret.incrementVersion();
}

void GHTransform::mult(const GHPoint16& lhs, const GHPoint16& rhs, GHPoint16& ret)
{
	GHPoint16 temp;

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			float sum = 0;
			for (int k = 0; k < 4; ++k) {
				sum += lhs[row*4+k] * rhs[k*4+col];
			}
			temp[row*4+col] = sum;
		}
	}
	ret = temp;
}

void GHTransform::mult(const GHPoint3& point, GHPoint3& ret) const
{
    
	float x = mMatrix[0]*point[0] + mMatrix[4]*point[1] + mMatrix[8]*point[2];
	float y = mMatrix[1]*point[0] + mMatrix[5]*point[1] + mMatrix[9]*point[2];
	float z = mMatrix[2]*point[0] + mMatrix[6]*point[1] + mMatrix[10]*point[2];
	
	x += mMatrix[12];
	y += mMatrix[13];
	z += mMatrix[14];
    
    float w = mMatrix[3]*point[0] + mMatrix[7]*point[1] + mMatrix[11]*point[2] + mMatrix[15];
    if (!GHFloat::isZero(w))
    {
        x /= w;
        y /= w;
        z /= w;
    }
    
	ret.setCoords(x,y,z);
}

void GHTransform::multDir(const GHPoint3& point, GHPoint3& ret) const
{
	float x = mMatrix[0]*point[0] + mMatrix[4]*point[1] + mMatrix[8]*point[2];
	float y = mMatrix[1]*point[0] + mMatrix[5]*point[1] + mMatrix[9]*point[2];
	float z = mMatrix[2]*point[0] + mMatrix[6]*point[1] + mMatrix[10]*point[2];
    
	ret.setCoords(x,y,z);
}

void GHTransform::setTranslate(const GHPoint3& translate)
{
	mMatrix[12] = translate[0];
	mMatrix[13] = translate[1];
	mMatrix[14] = translate[2];
	incrementVersion();
}

void GHTransform::getTranslate(GHPoint3& ret) const
{
    ret[0] = mMatrix[12];
    ret[1] = mMatrix[13];
    ret[2] = mMatrix[14];
}

void GHTransform::becomeIdentity(void)
{
    makeIdentity(mMatrix);
	incrementVersion();
}

void GHTransform::makeIdentity(GHPoint16& mat)
{
	for (int i = 0; i < 16; ++i) {
		mat[i] = 0;
	}
	mat[0] = 1;
	mat[5] = 1;
	mat[10] = 1;
	mat[15] = 1;
}

void GHTransform::becomeWorldTransform(const GHTransform& parent, const GHTransform& local)
{
	// calculate a version based on parent and local that is unlikely to collide.
	// 1000 was colliding in some cases that 997 does not.  997 was chosen because it is prime.
	short testVersion = parent.getVersion() * 997 + local.getVersion();
	if (testVersion != mVersion) {        
		local.mult(parent, *this);
        // we're out of date, merge the two transforms.
		mVersion = testVersion;
	}
}

void GHTransform::becomeXRotation(float angle)
{
	becomeIdentity();
	//1 0 0 
	//0 cos(a) -sin(a) 
	//0 sin(a) cos(a) 
	mMatrix[5] = cos(-angle);
	mMatrix[6] = -sin(-angle);
	mMatrix[9] = sin(-angle);
	mMatrix[10] = cos(-angle);
	incrementVersion();
}

void GHTransform::becomeYRotation(float angle)
{
	becomeIdentity();
	//cos(a) 0 sin(a) 
	//0 1 0 
	//-sin(a) 0 cos(a) 
	mMatrix[0] = cos(-angle);
	mMatrix[2] = sin(-angle);
	mMatrix[8] = -sin(-angle);
	mMatrix[10] = cos(-angle);
	incrementVersion();
}

void GHTransform::becomeZRotation(float angle)
{
	becomeIdentity();
	//cos(a) -sin(a) 0 
	//sin(a) cos(a) 0 
	//0 0 1 
	mMatrix[0] = cos(-angle);
	mMatrix[1] = -sin(-angle);
	mMatrix[4] = sin(-angle);
	mMatrix[5] = cos(-angle);
	incrementVersion();
}

void GHTransform::becomeAxisAngleRotation(const GHPoint3& axis, float angle)
{
	//formula taken fram http://en.wikipedia.org/wiki/Rotation_matrix#Axis_and_angle
	
	//c = cos(θ); s = sin(θ); C = 1-c
	//xs = x*s;   ys = y*s;   zs = z*s
	//xC = x*C;   yC = y*C;   zC = z*C
	//xyC = x*yC; yzC = y*zC; zxC = z*xC
	//[ x*xC+c   xyC-zs   zxC+ys ]
	//[ xyC+zs   y*yC+c   yzC-xs ]
	//[ zxC-ys   yzC+xs   z*zC+c ]
	
	
	float cosA = ::cos(angle), sinA = ::sin(angle);
	float invCos = 1.f - cosA;
	
	float xSinA = axis[0]*sinA;
	float ySinA = axis[1]*sinA;
	float zSinA = axis[2]*sinA;
	
	float xInvCos = axis[0]*invCos;
	float yInvCos = axis[1]*invCos;
	float zInvCos = axis[2]*invCos;
    
	float xyInvCos = xInvCos*axis[1];
	float yzInvCos = yInvCos*axis[2];
	float zxInvCos = zInvCos*axis[0];
	
	mMatrix[0] = axis[0]*xInvCos + cosA;
	mMatrix[1] = xyInvCos - zSinA;
	mMatrix[2] = zxInvCos + ySinA;
	mMatrix[3] = 0;
	
	mMatrix[4] = xyInvCos + zSinA;
	mMatrix[5] = axis[1]*yInvCos + cosA;
	mMatrix[6] = yzInvCos - xSinA;
	mMatrix[7] = 0;
	
	mMatrix[8] = zxInvCos - ySinA;
	mMatrix[9] = yzInvCos + xSinA;
	mMatrix[10] = axis[2]*zInvCos + cosA;
	mMatrix[11] = 0;
	
	mMatrix[12] = mMatrix[13] = mMatrix[14] = 0;
	mMatrix[15] = 1;
    
    incrementVersion();
}

void GHTransform::becomeScaleMatrix(float xScale, float yScale, float zScale)
{
	becomeIdentity();
	mMatrix[0] = xScale;
	mMatrix[5] = yScale;
	mMatrix[10] = zScale;
	incrementVersion();
}

void GHTransform::becomeYawPitchRollRotation(float yaw, float pitch, float roll)
{
    //formula taken from: http://planning.cs.uiuc.edu/node102.html
    
    float cosYaw = cos(yaw);
    float sinYaw = sin(yaw);
    float cosPitch = cos(pitch);
    float sinPitch = sin(pitch);
    float cosRoll = cos(roll);
    float sinRoll = sin(roll);
    
    mMatrix[0] = cosYaw*cosPitch;
    mMatrix[1] = cosYaw*sinPitch*sinRoll - sinYaw*cosRoll;
    mMatrix[2] = cosYaw*sinPitch*cosRoll + sinYaw*sinRoll;
    mMatrix[3] = 0;
    
    mMatrix[4] = sinYaw*cosPitch;
    mMatrix[5] = sinYaw*sinPitch*sinRoll + cosYaw*cosRoll;
    mMatrix[6] = sinYaw*sinPitch*cosRoll - cosYaw*sinRoll;
    mMatrix[7] = 0;
    
    mMatrix[8] = -sinPitch;
    mMatrix[9] = cosPitch*sinRoll;
    mMatrix[10] = cosPitch*cosRoll;
    mMatrix[11] = 0;
    
    mMatrix[12] = 0;
    mMatrix[13] = 0;
    mMatrix[14] = 0;
    mMatrix[15] = 1;

    incrementVersion();
}

void GHTransform::calcYawPitchRoll(float& yaw, float& pitch, float& roll) const
{
    // taken from bullet btMatrix3x3::getEulerZYX
    // Check that pitch is not at a singularity
    if (fabs(mMatrix[2*4+0]) >= 1)
    {
        yaw = 0;
        
        // From difference of angles formula
        float delta = atan2(mMatrix[0*4+0],mMatrix[0*4+2]);
        if (mMatrix[2*4+0] > 0)  //gimbal locked up
        {
            pitch = GHPI / 2.0f;
            roll = pitch + delta;
        }
        else // gimbal locked down
        {
            pitch = -GHHALFPI / 2.0f;
            roll = -pitch + delta;
        }
    }
    else
    {
        pitch = -asin(mMatrix[2*4+0]);
        float pitchCos = cos(pitch);
        roll = atan2(mMatrix[2*4+1]/pitchCos, mMatrix[2*4+2]/pitchCos);
        yaw = atan2(mMatrix[1*4+0]/pitchCos, mMatrix[0*4+0]/pitchCos);
    }
}

void GHTransform::becomeRotationBetweenVectors(const GHPoint3& sourceVec, const GHPoint3& destVec)
{
	//Algorithm taken from http://www.cs.brown.edu/~jfh/papers/Moller-EBA-1999/paper.pdf
	// note that their matrix format is the transpose of ours
	
	//this algorithm requires the source and destination vectors to be normalized
	assert(GHFloat::isEqual(sourceVec.lenSqr(), 1.f));
	assert(GHFloat::isEqual(destVec.lenSqr(), 1.f));
    
	float dotProd = sourceVec * destVec;
	
	//the two edge cases are where the vectors point in the same direction and where
	// they point directly away from each other. The magnitude of the cross product will
	// approach 0 in these cases and the algorithm will not work. Detect this via the dot product
	// and construct the matrix appropriately.
	if (GHFloat::isEqual(dotProd, 1.f ) ) {
		becomeIdentity();
		return;
	}
	if (GHFloat::isEqual(dotProd, -1.f) ) {
		becomeReflectionBetweenVectors(sourceVec, destVec);
		return;
	}
	
	
	GHPoint3 crossProd;
	GHMath::cross(sourceVec, destVec, crossProd);
	
	float h = (1 - dotProd) / (crossProd*crossProd);
    
	mMatrix[0] = dotProd + h*crossProd[0]*crossProd[0];
	mMatrix[4] = h*crossProd[0]*crossProd[1] - crossProd[2];
	mMatrix[8] = h*crossProd[0]*crossProd[2] + crossProd[1];
	mMatrix[3] = 0;
	
	mMatrix[1] = h*crossProd[0]*crossProd[1] + crossProd[2];
	mMatrix[5] = dotProd + h*crossProd[1]*crossProd[1];
	mMatrix[9] = h*crossProd[1]*crossProd[2] - crossProd[0];
	mMatrix[7] = 0;
	
	mMatrix[2] = h*crossProd[0]*crossProd[2] - crossProd[1];
	mMatrix[6] = h*crossProd[1]*crossProd[2] + crossProd[0];
	mMatrix[10] = dotProd + h*crossProd[2]*crossProd[2];
	mMatrix[11] = 0;
    
	mMatrix[12] = 0;
	mMatrix[13] = 0;
	mMatrix[14] = 0;
	mMatrix[15] = 1;
    
    incrementVersion();
}

void GHTransform::becomeReflectionBetweenVectors(const GHPoint3& sourceVec, const GHPoint3& destVec)
{
	//Algorithm taken from http://www.cs.brown.edu/~jfh/papers/Moller-EBA-1999/paper.pdf,
	// where it is presented as a backup algorithm for the edge case in the arbitrary rotation matrix
    
	GHPoint3 absSource( ::fabs(sourceVec[0]), ::fabs(sourceVec[1]), ::fabs(sourceVec[2]) );
	
	GHPoint3 orthoAxis;
	if( absSource[0] <= absSource[1] && absSource[0] <= absSource[2] ) {
		orthoAxis.setCoords(1,0,0);
	}
	else if(absSource[1] <= absSource[2]) {
		orthoAxis.setCoords(0,1,0);
	}
	else {
		orthoAxis.setCoords(0,0,1);
	}
	
	GHPoint3 fromSource = orthoAxis;
	fromSource -= sourceVec;
	GHPoint3 fromDest = orthoAxis;
	fromDest -= destVec;
	
	float multiplier1 = 2.f / (fromSource*fromSource);
	float multiplier2 = 2.f / (fromDest*fromDest);
	float multiplier3 = 4.f * (fromSource*fromDest) / ((fromSource*fromSource)*(fromDest*fromDest));
	
	for(int i = 0; i < 3; ++i) 
	{
		for(int j = 0; j < 3; ++j)
		{
			float delta = i == j ? 1.0f : 0.0f;
			
			mMatrix[j*4 + i] = delta 
            - multiplier1*fromSource[i]*fromSource[j]
            - multiplier2*fromDest[i]*fromDest[j]
            + multiplier3*fromDest[i]*fromSource[j];
		}
	}
    
	mMatrix[3] = 0;
	mMatrix[7] = 0;
	mMatrix[11] = 0;
	mMatrix[12] = 0;
	mMatrix[13] = 0;
	mMatrix[14] = 0;
	mMatrix[15] = 1;
    
	incrementVersion();
}

void GHTransform::uniformScale(float scale)
{
	for (size_t i = 0; i < 12; ++i) {
		mMatrix[i] *= scale;
	}
	incrementVersion();
}

// used for invert, taken from http://www.flipcode.com/documents/matrfaq.html#Q17
// makes a 3x3 matrix from the offset (i,j)
static void m4_submat(GHTransform& src, GHPoint<float, 9>& mb, int i, int j)
{
    int ti, tj, idst, jdst;
    
    for ( ti = 0; ti < 4; ti++ )
    {
        if ( ti < i )
            idst = ti;
        else
            if ( ti > i )
                idst = ti-1;
        
        for ( tj = 0; tj < 4; tj++ )
        {
            if ( tj < j )
                jdst = tj;
            else
                if ( tj > j )
                    jdst = tj-1;
            
            if ( ti != i && tj != j )
                mb[idst*3 + jdst] = src.getMatrix()[ti*4 + tj ];
        }
    }
}

// taken from http://www.flipcode.com/documents/matrfaq.html#Q17
void GHTransform::invert(void)
{
    float mdet = calculateDeterminant();
    GHPoint<float, 9> mtemp;
    int sign;

    if (GHFloat::isZero(mdet))
    {
		GHMATHDEBUGPRINT("GHTransform::invert() mdet is zero.");
        //assert(!GHFloat::isZero(mdet) && "Matrices with 0 determinants are not invertable!");
		return;
	}

    GHPoint<float, 16> dest;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            sign = 1 - ( (i +j) % 2 ) * 2;
            m4_submat(*this, mtemp, i, j);
            dest[i+j*4] = (GHMath::calcDeterminant3x3(mtemp) * sign) / mdet;
        }
    }
    
    mMatrix = dest;
    
    incrementVersion();
}

float GHTransform::calculateDeterminant(void) const
{
    //determinant of a 4x4 matrix,
    //from http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
    
    const double a11 = mMatrix[0];
    const double a12 = mMatrix[1];
    const double a13 = mMatrix[2];
    const double a14 = mMatrix[3];
    const double a21 = mMatrix[4];
    const double a22 = mMatrix[5];
    const double a23 = mMatrix[6];
    const double a24 = mMatrix[7];
    const double a31 = mMatrix[8];
    const double a32 = mMatrix[9];
    const double a33 = mMatrix[10];
    const double a34 = mMatrix[11];
    const double a41 = mMatrix[12];
    const double a42 = mMatrix[13];
    const double a43 = mMatrix[14];
    const double a44 = mMatrix[15];
    
    return  (float)	(a11*a22*a33*a44 + a11*a23*a34*a42 + a11*a24*a32*a43 +
					a12*a21*a34*a43 + a12*a23*a31*a44 + a12*a24*a33*a41 +
					a13*a21*a32*a44 + a13*a22*a34*a41 + a13*a24*a31*a42 +
					a14*a21*a33*a42 + a14*a22*a31*a43 + a14*a23*a32*a41 -
					a11*a22*a34*a43 - a11*a23*a32*a44 - a11*a24*a33*a42 -
					a12*a21*a33*a44 - a12*a23*a34*a41 - a12*a24*a31*a43 -
					a13*a21*a34*a42 - a13*a22*a31*a44 - a13*a24*a32*a41 -
					a14*a21*a32*a43 - a14*a22*a33*a41 - a14*a23*a31*a42);
            
}

void GHTransform::getTranspose(GHTransform& outTranspose) const
{
	GHMath::transposeMatrix4(mMatrix.getCoords(), outTranspose.getMatrix().getArr());
	outTranspose.incrementVersion();
}

// modified from http://iphonedevelopment.blogspot.com/2008/12/glulookat.html
// and http://www.khronos.org/message_boards/viewtopic.php?t=541
static void CrossProd(float x1, float y1, float z1, float x2, float y2, float z2, float res[3]) 
{ 
	res[0] = y1*z2 - y2*z1; 
	res[1] = x2*z1 - x1*z2; 
	res[2] = x1*y2 - x2*y1; 
} 
void GHTransform::lookAt(float eyeX, float eyeY, float eyeZ,
                         float lookAtX, float lookAtY, float lookAtZ,
                         float upX, float upY, float upZ)
{
	float f[3]; 
	
	// calculating the viewing vector 
	f[0] = lookAtX - eyeX; 
	f[1] = lookAtY - eyeY; 
	f[2] = lookAtZ - eyeZ; 
	
	double fMag, upMag; 
	fMag = ::sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]); 
	upMag = ::sqrt(upX*upX + upY*upY + upZ*upZ); 
	
	// normalizing the viewing vector 
	if( fMag != 0) 
	{ 
		f[0] = (float)(f[0]/fMag); 
		f[1] = (float)(f[1]/fMag); 
		f[2] = (float)(f[2]/fMag); 
	} 
	
	// normalising the up vector. no need for this here if you have your 
	// up vector already normalised, which is mostly the case. 
	if( upMag != 0 ) 
	{ 
		upX = (float)(upX/upMag); 
		upY = (float)(upY/upMag); 
		upZ = (float)(upZ/upMag); 
	} 
	
	float s[3], u[3]; 
	
	CrossProd(f[0], f[1], f[2], upX, upY, upZ, s); 
	CrossProd(s[0], s[1], s[2], f[0], f[1], f[2], u); 
	float M[]= 
	{ 
		-s[0], -s[1], -s[2], 0, 
		u[0], u[1], u[2], 0, 
		f[0], f[1], f[2], 0, 
		eyeX, eyeY, eyeZ, 1 
	};
	
	mMatrix.setCoords(M);
	incrementVersion();
}
void GHTransform::lookAt(const GHPoint3& eyePos, const GHPoint3& centerPos)
{
    GHPoint3 camDir(centerPos);
    camDir -= eyePos;
    camDir.normalize();
    
    GHPoint3 upDir(0,1,0);
    GHPoint3 camLeft;
    GHMath::cross(upDir, camDir, camLeft);
    GHMath::cross(camDir, camLeft, upDir);
    
    lookAt(eyePos[0], eyePos[1], eyePos[2],
           centerPos[0], centerPos[1], centerPos[2],
           upDir[0], upDir[1], upDir[2]);
}

// taken from
// http://www.google.com/codesearch/p?hl=en#uX1GffpyOZk/opengl/libagl/fp.h&q=mul2f&sa=N&cd=1&ct=rc&l=207
static float addExpf(float v, int e) {
    union {
        float       f;
        int		    i;
    };
    f = v;
    if (i<<1) { 
        i += e<<23;
    }
    return f;
}
static float mul2f(float v) {
    return addExpf(v, 1);
}

// taken from
// http://www.google.com/codesearch/p?hl=en#uX1GffpyOZk/opengl/libagl/matrix.cpp&q=glfrustumf%20lang:c++&sa=N&cd=1&ct=rc&l=7
void GHTransform::frustumf(float left, float right, float bottom, float top, float zNear, float zFar)
{
    const float r_width = 1.0f / (right - left);
    const float r_height = 1.0f / (top - bottom);
    const float r_depth = 1.0f / (zNear - zFar);
    const float x = mul2f(zNear * r_width);
    const float y = mul2f(zNear * r_height);
    const float A = mul2f((right + left) * r_width);
    const float B = (top + bottom) * r_height;
    const float C = (zFar + zNear) * r_depth;
    const float D = mul2f(zFar * zNear * r_depth);
    
    mMatrix[ 0] = x;
    mMatrix[ 5] = y;
    mMatrix[ 8] = A;
    mMatrix[ 9] = B;
    mMatrix[10] = C;
    mMatrix[14] = D;
    mMatrix[11] = -1.0f;
    mMatrix[ 1] = mMatrix[ 2] = mMatrix[ 3] =
    mMatrix[ 4] = mMatrix[ 6] = mMatrix[ 7] =
    mMatrix[12] = mMatrix[13] = mMatrix[15] = 0.0f;
    
	incrementVersion();
}

// https://github.com/SVMP/android_frameworks_base/blob/master/opengl/libagl/matrix.cpp
void GHTransform::orthof(float left, float right, float bottom, float top, float zNear, float zFar)
{
	if (left == right || top == bottom || zNear == zFar) 
	{
		GHMATHDEBUGPRINT("Bad values for orthof");
		return;
	}
    
    const float r_width  = 1.0f / (right - left);
    const float r_height = 1.0f / (top - bottom);
    const float r_depth  = 1.0f / (zFar - zNear);
    const float x =  mul2f(r_width);
    const float y =  mul2f(r_height);
    const float z = -mul2f(r_depth);
    const float tx = -(right + left) * r_width;
    const float ty = -(top + bottom) * r_height;
    const float tz = -(zFar + zNear) * r_depth;
    
    mMatrix[ 0] = x;
    mMatrix[ 5] = y;
    mMatrix[10] = z;
    mMatrix[12] = tx;
    mMatrix[13] = ty;
    mMatrix[14] = tz;
    mMatrix[15] = 1.0f;
    mMatrix[ 1] = mMatrix[ 2] = mMatrix[ 3] =
    mMatrix[ 4] = mMatrix[ 6] = mMatrix[ 7] =
    mMatrix[ 8] = mMatrix[ 9] = mMatrix[11] = 0.0f;
	
	incrementVersion();
}

void GHTransform::interpolateTo(const GHTransform& dest, float interpVal, GHTransform& outVal) const
{
    GHPoint3 srcPt, destPt, interpPt;
    GHPoint4 srcQuat, destQuat, interpQuat;
    
    getTranslate(srcPt);
    GHQuaternion::convertTransformToQuaternion(*this, srcQuat);
    
    dest.getTranslate(destPt);
    GHQuaternion::convertTransformToQuaternion(dest, destQuat);
    
    srcPt.linearInterpolate(destPt, interpVal, interpPt);
    GHQuaternion::slerp(srcQuat, destQuat, interpVal, interpQuat);
    
    GHQuaternion::convertQuaternionToTransform(interpQuat, outVal);
    outVal.setTranslate(interpPt);
}
