// Copyright Golden Hammer Software
#pragma once

#include "GHPoint.h"

// Represents a 4x4 matrix
// We expect any direct access to the values to be followed by an incrementVersion()
class GHTransform
{
public:
    static GHTransform IDENTITY;
    static const int INITIAL_VERSION = 1;
    
public:
    GHTransform(void);
    GHTransform(float f1, float f2, float f3, float f4,
                float f5, float f6, float f7, float f8, 
                float f9, float f10, float f11, float f12,
                float f13, float f14, float f15, float f16);

	GHTransform(const GHPoint16& matrix);

    const GHPoint16& getMatrix(void) const { return mMatrix; }
    GHPoint16& getMatrix(void) { return mMatrix; }
    
    short getVersion(void) const { return mVersion; }
    void incrementVersion(void) { mVersion++; }
    void resetVersion(void) { mVersion = INITIAL_VERSION; }
    
    void mult(const GHTransform& other, GHTransform& ret) const;
    void mult(const GHPoint3& first, GHPoint3& ret) const;
    // mult without taking into account our translate
    void multDir(const GHPoint3& first, GHPoint3& ret) const;

    void setTranslate(const GHPoint3& translate);
	void getTranslate(GHPoint3& ret) const;

    void becomeIdentity(void);
    static void makeIdentity(GHPoint16& mat);
    void becomeWorldTransform(const GHTransform& parent, const GHTransform& local);
	void becomeXRotation(float angle);
	void becomeYRotation(float angle);
	void becomeZRotation(float angle);
	void becomeAxisAngleRotation(const GHPoint3& axis, float angle);
	void becomeScaleMatrix(float xScale, float yScale, float zScale);
    // actually roll/yaw/pitch
	void becomeYawPitchRollRotation(float yaw, float pitch, float roll);
    // actually roll/yaw/pitch
	void calcYawPitchRoll(float& yaw, float& pitch, float& roll) const;
	void becomeRotationBetweenVectors(const GHPoint3& sourceVec, const GHPoint3& destVec);
	void becomeReflectionBetweenVectors(const GHPoint3& sourceVec, const GHPoint3& destVec);

	void uniformScale(float scale);
    void invert(void);
    void getTranspose(GHTransform& outTranspose) const;

    void lookAt(float eyex, float eyey, float eyez,
                float centerx, float centery, float centerz,
                float upx, float upy, float upz);
    // calculate a decent up vector and call lookAt.
    void lookAt(const GHPoint3& eyePos, const GHPoint3& centerPos);
	void frustumf(float left, float right, float bottom, float top, float zNear, float zFar);
	void orthof(float left, float right, float bottom, float top, float zNear, float zFar);

    GHTransform& operator=(const GHTransform& other) { mMatrix = other.getMatrix(); incrementVersion(); return *this; }
    float& operator[](unsigned int i) { return mMatrix[i]; }
    const float& operator[](unsigned int i) const { return mMatrix[i]; }
    
    void interpolateTo(const GHTransform& dest, float interpVal, GHTransform& outVal) const;

	static void mult(const GHPoint16& lhs, const GHPoint16& rhs, GHPoint16& ret);
    
private:
	float calculateDeterminant(void) const;
	float calculateSubDeterminant(int i11, int i12, int i21, int i22) const;
    
private:
    GHPoint16 mMatrix;
    // version id, incremented every time something changes in our array
	// a world transform is valid if the version is equal to parent's version + local version.
    short mVersion;
};
