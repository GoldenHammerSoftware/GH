// Copyright Golden Hammer Software
#pragma once

#include "GHParticle.h"
#include "GHParticleSystemDesc.h"
#include "GHParticleSpawner.h"
#include "GHVertexBuffer.h"
#include <vector>

class GHTimeVal;
class GHVertexBuffer;
class GHTransform;
class GHFrustum;
class GHViewInfo;

// a group of particles that update and render together
// the system spawns new particles.
class GHParticleSystem
{
public:
	GHParticleSystem(GHParticleSystemDescResource* desc);
	~GHParticleSystem(void);

	void update(const GHTimeVal& time, const GHViewInfo& viewInfo);

	// checkDead is called every frame.  query the results.
	bool isDead(void) const { return mIsDead; }
	// make a particle system come back from the dead for another run.
	void revive(void) { mDieTime = -1; mIsDead = false; }
	void die(void) { mIsDead = true; }
	bool isEmpty(void) const { return (mNumActiveParticles == 0); }

	void setTargBuffer(GHVertexBufferPtr* buffer);
	// root pos is the centerpoint of spawning new particles
	void setRootPos(const GHPoint3& pos) { mRootPos = pos; }
	// offset pos is a value added to every particle for rendering.
	void setOffsetPos(const GHPoint3& pos) { mOffsetPos = pos; }

	GHParticleSystemDescResource* getDesc(void) { return mDesc; }
	GHParticleSpawner& getSpawner(void) { return mSpawner; }

	// only for use by GHParticleSpawner.
	GHParticle* getNewEmptyParticle(void);

	void calcFrameUpdateInfo(float timeSinceLastUpdate, float& radiusInc, float& alphaMinus, float& velMod, GHPoint3& velAdd);
	void updateParticle(GHParticle& particle, const GHFrustum* frustum,
		float velMod, float alphaMinus, float radiusInc,
		const GHPoint3& velAdd,
		float timeSinceLastUpdate, float currentTime);

protected:
	void init(void);

	void updateParticles(const GHTimeVal& time, const GHFrustum& frustum);
	// if the system has a limited lifetime, see if we're dead yet.
	void checkDead(const GHTimeVal& time);

	void updateBuffer(GHTransform billboardTrans);
	GHVertexBufferPtr* getTargetBuffer(void) { return mTargBuffer; }

	void checkParticleLifetime(GHParticle& particle, const float currentTime);
	void killParticle(GHParticle& particle);

protected:
	GHParticleSystemDescResource* mDesc;
	std::vector<GHParticle*> mParticles;
	unsigned int mNumActiveParticles;
	int mNextNewParticleIdx;

	GHVertexBufferPtr* mTargBuffer;
	// the point where particles are spawned from.
	GHPoint3 mRootPos;
	// an offset pos that we add to each particle while adding it to the buffer.
	// used when particle system is not in world space.  
	// root pos is set to 0 and offset is current parent position.
	GHPoint3 mOffsetPos;

	float mDieTime;
	float mLastUpdateTime;
	bool mIsDead;

	GHParticleSpawner mSpawner;
};
