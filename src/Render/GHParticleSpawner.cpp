// Copyright Golden Hammer Software
#include "GHParticleSpawner.h"
#include "GHParticleSystem.h"
#include "GHParticleSystemDesc.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHMath/GHRandom.h"

GHParticleSpawner::GHParticleSpawner(GHParticleSystem& system, const GHPoint3& rootPos)
: mSystem(system)
, mRootPos(rootPos)
, mNextParticleSpawn(-1)
, mSpawningAllowed(true)
, mHasLastRootPos(false)
{
}

void GHParticleSpawner::setSpawningAllowed(bool val)
{
	if (mSpawningAllowed != val)
	{
		mSpawningAllowed = val;
		mNextParticleSpawn = -1;
		mHasLastRootPos = false;
	}
}

void GHParticleSpawner::spawnNewParticles(const GHTimeVal& time)
{
	if (!mSpawningAllowed) return;

	if (mNextParticleSpawn == -1) {
		// first update.
		mNextParticleSpawn = time.getTime() - 0.001f;
	}

	float currSpawnTime = mNextParticleSpawn;
	unsigned int numToSpawn = 0;
	while (mNextParticleSpawn < time.getTime()) 
	{
		numToSpawn++;
		pickNextParticleSpawnTime();
		if (numToSpawn >= this->mSystem.getDesc()->get()->getMaxParticles())
		{
			mNextParticleSpawn = time.getTime();
			break;
		}
	}
	if (!numToSpawn) {
		return;
	}
	// spawn the actual particles
	// if we have a very high spawn rate (faster than one frame) then interpolate the spawn pos.
	GHPoint3 posChange = mRootPos;
	if (!mHasLastRootPos) {
		mLastRootPos = mRootPos;
		mHasLastRootPos = true;
		mHasLastRootVel = false;
	}

	posChange -= mLastRootPos;
	GHPoint3 rootVel = posChange;
	rootVel /= time.getTimePassed();
	rootVel *= mSystem.getDesc()->get()->getInheritVelPct();

	if (!mHasLastRootVel) {
		mLastRootVel = rootVel;
		mHasLastRootVel = true;
	}

	GHPoint3 velChange = rootVel;
	velChange -= mLastRootVel;
	velChange /= (float)numToSpawn;
	GHPoint3 inheritVel = mLastRootVel;

	float totSpawnTime = time.getTime() - currSpawnTime;
	float timeInc = totSpawnTime / numToSpawn;
	posChange /= (float)numToSpawn;
	GHPoint3 spawnPos = mLastRootPos;
	float currTimeInc = totSpawnTime;

	for (unsigned int i = 0; i < numToSpawn; ++i) {
		spawnPos += posChange;
		inheritVel += velChange;
		GHParticle* part = spawnParticle(currSpawnTime, spawnPos, inheritVel);
		if (!part) break;

		float radiusInc, alphaMinus, velMod;
		GHPoint3 velAdd;
		mSystem.calcFrameUpdateInfo(currTimeInc, radiusInc, alphaMinus, velMod, velAdd);
		mSystem.updateParticle(*part, 0, velMod, alphaMinus, radiusInc, velAdd, currTimeInc, currSpawnTime);

		currTimeInc -= timeInc;
		currSpawnTime += timeInc;
	}
	mLastRootVel = rootVel;
	mLastRootPos = mRootPos;
}

void GHParticleSpawner::pickNextParticleSpawnTime(void)
{
	if (mSystem.getDesc()->get()->getSpawnRate() == 0) {
		return;
	}

	mNextParticleSpawn += mSystem.getDesc()->get()->getSpawnRate() + GHRandom::getNormalizedRandVal()*mSystem.getDesc()->get()->getSpawnRateRandom();
}

GHParticle* GHParticleSpawner::spawnParticle(float spawnTime, const GHPoint3& spawnPos, const GHPoint3& inheritVel)
{
	GHParticle* part = mSystem.getNewEmptyParticle();
	if (!part) return 0;
	part->reset();
	part->setStartTime(spawnTime);
	part->setEndTime(spawnTime +
		mSystem.getDesc()->get()->getParticleLifetime() + GHRandom::getNormalizedRandVal()*mSystem.getDesc()->get()->getParticleLifetimeRand());
	part->setDead(false);
	part->setRadius(mSystem.getDesc()->get()->getParticleRadius());

	GHPoint3 realPos = spawnPos;
	for (int i = 0; i < 3; ++i)
	{
		realPos[i] += mSystem.getDesc()->get()->getSpawnPosOffset()[i];
		realPos[i] += -mSystem.getDesc()->get()->getSpawnPosRandom()[i] + GHRandom::getNormalizedRandVal()*(mSystem.getDesc()->get()->getSpawnPosRandom()[i] * 2);
	}
	part->setPosition(realPos);

	GHPoint3 vel;
	mSystem.getDesc()->get()->calcInitialVelocity(vel);
	vel += inheritVel;
	part->setVelocity(vel);

	float rotation = mSystem.getDesc()->get()->getRotation() + GHRandom::getNormalizedRandVal()*(mSystem.getDesc()->get()->getRotation() + mSystem.getDesc()->get()->getRotationRandom());
	part->setRotationMod(rotation);
	float initialRotation = GHRandom::getNormalizedRandVal()*mSystem.getDesc()->get()->getInitialRotationRandom();
	part->setCurrRotation(initialRotation);

	return part;
}
