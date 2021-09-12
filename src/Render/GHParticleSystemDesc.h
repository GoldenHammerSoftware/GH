// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHUtils/GHResource.h"

class GHMaterial;

// set of variables that describes how a particle system should behave.
class GHParticleSystemDesc
{
public:
	GHParticleSystemDesc(void);
	GHParticleSystemDesc(const GHParticleSystemDesc& other);
	~GHParticleSystemDesc(void);

	// apply our starter vals and randoms to pick an initial direction and speed for a particle.
	void calcInitialVelocity(GHPoint3& ret);

	GHMaterial* getMaterial(void) const { return mMaterial; }
	void setMaterial(GHMaterial* mat);

	unsigned int getMaxParticles(void) const { return mMaxParticles; }
	void setMaxParticles(unsigned int val) { mMaxParticles = val; }

	float getSpawnRate(void) const { return mSpawnRate; }
	void setSpawnRate(float val) { mSpawnRate = val; }
	float getSpawnRateRandom(void) const { return mSpawnRateRandom; }
	void setSpawnRateRandom(float val) { mSpawnRateRandom = val; }

	float getParticleLifetime(void) const { return mParticleLifetime; }
	void setParticleLifetime(float val) { mParticleLifetime = val; }
	float getParticleLifetimeRand(void) const { return mParticleLifetimeRand; }
	void setParticleLifetimeRand(float val) { mParticleLifetimeRand = val; }

	float getParticleRadius(void) const { return mParticleRadius; }
	void setParticleRadius(float val) { mParticleRadius = val; }
	float getRadiusGrowth(void) const { return mRadiusGrowth; }
	void setRadiusGrowth(float val) { mRadiusGrowth = val; }

	void setInitialPitch(float val) { mInitialPitch = val; }
	float getInitialPitch(void) const { return mInitialPitch; }
	void setInitialPitchRand(float val) { mInitialPitchRand = val; }
	float getInitialPitchRand(void) const { return mInitialPitchRand; }
	void setInitialYaw(float val) { mInitialYaw = val; }
	float getInitialYaw(void) const { return mInitialYaw; }
	void setInitialYawRand(float val) { mInitialYawRand = val; }
	float getInitialYawRand(void) const { return mInitialYawRand; }
	void setInitialVel(float val) { mInitialVel = val; }
	float getInitialVel(void) const { return mInitialVel; }
	void setInitialVelRand(float val) { mInitialVelRand = val; }
	float getInitialVelRand(void) const { return mInitialVelRand; }

	float getInheritVelPct(void) const { return mInheritVelPct; }
	void setInheritVelPct(float val) { mInheritVelPct = val; }

	float getLifetime(void) const { return mLifetime; }
	void setLifetime(float val) { mLifetime = val; }

	float getAlphaMod(void) const { return mAlphaMod; }
	void setAlphaMod(float val) { mAlphaMod = val; }
	float getInitialAlpha(void) const { return mInitialAlpha; }
	void setInitialAlpha(float val) { mInitialAlpha = val; }

	float getFriction(void) const { return mFriction; }
	void setFriction(float val) { mFriction = val; }

	const GHPoint3& getSpawnPosRandom(void) const { return mSpawnPosRandom; }
	void setSpawnPosRandom(const GHPoint3& pos) { mSpawnPosRandom = pos; }
	const GHPoint3& getSpawnPosOffset(void) const { return mSpawnPosOffset; }
	void setSpawnPosOffset(const GHPoint3& pos) { mSpawnPosOffset = pos; }

	const GHPoint3& getGravity(void) const { return mGravity; }
	void setGravity(const GHPoint3& gravity) { mGravity = gravity; }

	float getRotation(void) const { return mRotation; }
	void setRotation(float val) { mRotation = val; }
	float getRotationRandom(void) const { return mRotationRand; }
	void setRotationRandom(float val) { mRotationRand = val; }
	void setInitialRotationRandom(float val) { mInitialRotationRandom = val; }
	float getInitialRotationRandom(void) { return mInitialRotationRandom; }

	bool getIsWorldSpace(void) const { return mIsWorldSpace; }
	void setIsWorldSpace(bool val) { mIsWorldSpace = val; }

	bool getBillboardUp(void) const { return mBillboardUp; }
	void setBillboardUp(bool val) { mBillboardUp = val; }

protected:
	GHMaterial* mMaterial;

	GHPoint3 mGravity;

	// number of particles spawned per second.
	float mSpawnRate;
	float mSpawnRateRandom;

	unsigned int mMaxParticles;

	// length of time particles live.
	float mParticleLifetime;
	float mParticleLifetimeRand;

	// initial direction of travel of new particles
	float mInitialPitch;
	float mInitialPitchRand;
	float mInitialYaw;
	float mInitialYawRand;
	float mInitialVel;
	float mInitialVelRand;
	float mInheritVelPct;

	float mParticleRadius;
	float mRadiusGrowth;

	float mAlphaMod;
	float mInitialAlpha;

	float mRotation;
	float mRotationRand;
	float mInitialRotationRandom;

	// set to 0 for infinite lifetime of the system.s
	float mLifetime;

	// defines how the individual particles slow down or speed up over time.
	float mFriction;

	// says whether the whole particle system follows a root node or not
	bool mIsWorldSpace;

	// flag to use the billboardUp transform instead of the regular billboard.
	bool mBillboardUp;

	GHPoint3 mSpawnPosRandom;
	GHPoint3 mSpawnPosOffset;
};

typedef GHResourcePtr<GHParticleSystemDesc> GHParticleSystemDescResource;
