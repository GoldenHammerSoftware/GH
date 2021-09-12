// Copyright Golden Hammer Software
#include "GHParticleSystemDesc.h"
#include "GHMaterial.h"
#include "GHMath/GHRandom.h"
#include "GHMath/GHTransform.h"

GHParticleSystemDesc::GHParticleSystemDesc(void)
: mMaterial(0)
, mSpawnRate(0)
, mSpawnRateRandom(0)
, mMaxParticles(0)
, mParticleLifetime(0)
, mParticleLifetimeRand(0)
, mInitialPitch(0)
, mInitialPitchRand(0)
, mInitialYaw(0)
, mInitialYawRand(0)
, mInitialVel(1.0f)
, mInitialVelRand(0)
, mLifetime(-1)
, mRadiusGrowth(1)
, mAlphaMod(0)
, mInitialAlpha(1)
, mFriction(0)
, mInheritVelPct(1)
, mRotation(0)
, mRotationRand(0)
, mIsWorldSpace(true)
, mBillboardUp(false)
, mInitialRotationRandom(0)
{
}

GHParticleSystemDesc::GHParticleSystemDesc(const GHParticleSystemDesc& other)
: mMaterial(other.mMaterial)
, mSpawnRate(other.mSpawnRate)
, mSpawnRateRandom(other.mSpawnRateRandom)
, mMaxParticles(other.mMaxParticles)
, mParticleLifetime(other.mParticleLifetime)
, mParticleLifetimeRand(other.mParticleLifetimeRand)
, mInitialPitch(other.mInitialPitch)
, mInitialPitchRand(other.mInitialPitchRand)
, mInitialYaw(other.mInitialYaw)
, mInitialYawRand(other.mInitialYawRand)
, mInitialVel(other.mInitialVel)
, mInitialVelRand(other.mInitialVelRand)
, mLifetime(other.mLifetime)
, mRadiusGrowth(other.mRadiusGrowth)
, mAlphaMod(other.mAlphaMod)
, mFriction(other.mFriction)
, mInitialAlpha(other.mInitialAlpha)
, mInheritVelPct(other.mInheritVelPct)
, mRotation(other.mRotation)
, mRotationRand(other.mRotationRand)
, mIsWorldSpace(other.mIsWorldSpace)
, mBillboardUp(other.mBillboardUp)
, mSpawnPosRandom(other.mSpawnPosRandom)
, mSpawnPosOffset(other.mSpawnPosOffset)
, mInitialRotationRandom(other.mInitialRotationRandom)
{
	// we just got a handle to a material from another description, so claim the ref count.
	if (mMaterial) mMaterial->acquire();
}

GHParticleSystemDesc::~GHParticleSystemDesc(void)
{
	GHRefCounted::changePointer((GHRefCounted*&)mMaterial, 0);
}

void GHParticleSystemDesc::setMaterial(GHMaterial* mat)
{
	GHRefCounted::changePointer((GHRefCounted*&)mMaterial, mat);
}

void GHParticleSystemDesc::calcInitialVelocity(GHPoint3& ret)
{
	ret.setCoords(0, 1, 0);
	GHTransform rotMat;

	float pitchSize = mInitialPitch + GHRandom::getNormalizedRandVal()*mInitialPitchRand - mInitialPitchRand/2.0f;
	if (pitchSize)
	{
		rotMat.becomeXRotation(pitchSize);
		rotMat.mult(ret, ret);
	}

	float yawSize = mInitialYaw + GHRandom::getNormalizedRandVal()*mInitialYawRand - mInitialYawRand/2.0f;
	if (yawSize != 0)
	{
		rotMat.becomeYRotation(yawSize);
		rotMat.mult(ret, ret);
	}

	float velSize = mInitialVel + GHRandom::getNormalizedRandVal()*mInitialVel;
	ret *= velSize;
}
