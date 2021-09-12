// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

class GHParticleSystem;
class GHTimeVal;
class GHParticle;

// widget class to manage spawning new particles.
class GHParticleSpawner
{
public:
	GHParticleSpawner(GHParticleSystem& system, const GHPoint3& rootPos);

	// allow/disable spawning new sparticles.
	void setSpawningAllowed(bool val);
	void spawnNewParticles(const GHTimeVal& time);

private:
	void pickNextParticleSpawnTime(void);
	GHParticle* spawnParticle(float spawnTime, const GHPoint3& spawnPos, const GHPoint3& inheritVel);

private:
	GHParticleSystem& mSystem;

	// centerpoint of particle spawning new particles
	const GHPoint3& mRootPos;

	// we keep track of previous root position for calculating vel
	GHPoint3 mLastRootPos;
	// flag to say whether last root pos is valid yet.  not valid until second update.
	bool mHasLastRootPos;
	// velocity used for inherit vel from parent.
	GHPoint3 mLastRootVel;
	// is last root vel valid yet.
	bool mHasLastRootVel;

	// time of next particle spawn.
	float mNextParticleSpawn;
	// flag to enable/disable spawning new particles
	bool mSpawningAllowed;
};

