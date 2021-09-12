// Copyright Golden Hammer Software
#include "GHParticleSystem.h"
#include "GHGeometry.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHVertexBuffer.h"
#include "GHMaterial.h"
#include "GHViewInfo.h"
#include "GHParticleSort.h"
#include "GHPlatform/GHDebugMessage.h"
#include <algorithm>
#include "GHParticleSpawner.h"
#include <stdint.h>
#include "GHMath/GHPlane.h"
#include "GHMath/GHColor.h"

GHParticleSystem::GHParticleSystem(GHParticleSystemDescResource* desc)
: mNumActiveParticles(0)
, mIsDead(false)
, mDieTime(-1)
, mTargBuffer(0)
, mNextNewParticleIdx(0)
, mSpawner(*this, mRootPos)
, mLastUpdateTime(0)
, mDesc(0)
, mOffsetPos(0,0,0)
{
	GHRefCounted::changePointer((GHRefCounted*&)mDesc, desc);
	assert(mDesc && mDesc->get());
	init();
}

GHParticleSystem::~GHParticleSystem(void)
{
	for (unsigned int i = 0; i < mParticles.size(); ++i) {
		GHParticle* dummyPart = mParticles[i];
		delete dummyPart;
	}
	mParticles.clear();

	if (mTargBuffer) {
		mTargBuffer->release();
		mTargBuffer = 0;
	}
    
    GHRefCounted::changePointer((GHRefCounted*&)mDesc, 0);
}

void GHParticleSystem::init(void)
{
	// fill up our vector with enough pre-instantiated particles to use.
	mParticles.reserve(mDesc->get()->getMaxParticles());
	for (unsigned int i = 0; i < mDesc->get()->getMaxParticles(); ++i) {
		GHParticle* dummyPart = new GHParticle;
		dummyPart->setInitialAlpha(mDesc->get()->getInitialAlpha());
		mParticles.push_back(dummyPart);
	}
}

void GHParticleSystem::update(const GHTimeVal& time, const GHViewInfo& viewInfo)
{
	if (mLastUpdateTime == time.getTime()) return;
	mLastUpdateTime = time.getTime();

	checkDead(time);
	if (!isEmpty()) {
		updateParticles(time, viewInfo.getFrustum());

		if (mDesc->get()->getMaterial() && mDesc->get()->getMaterial()->isAlpha()) {
			GHRenderParticleLessBackToFront sorter;
			std::sort(mParticles.begin(), mParticles.end(), sorter);
		}
		else {
			GHRenderParticleLessFrontToBack sorter;
			std::sort(mParticles.begin(), mParticles.end(), sorter);
		}

	}
	if (!mIsDead) {
		mSpawner.spawnNewParticles(time);
	}

	if (mDesc->get()->getBillboardUp()) {
		updateBuffer(viewInfo.getEngineTransforms().mBillboardUpTransform);
	}
	else {
		updateBuffer(viewInfo.getEngineTransforms().mBillboardTransform);
	}
}

GHParticle* GHParticleSystem::getNewEmptyParticle(void)
{
	if (mNumActiveParticles < mDesc->get()->getMaxParticles()) {
		mNextNewParticleIdx = mNumActiveParticles;
		mNumActiveParticles++;
	}
	else if (mNextNewParticleIdx >= (int)mDesc->get()->getMaxParticles()) {
		mNextNewParticleIdx = 0;
	}
	GHParticle* ret = mParticles[mNextNewParticleIdx];
	mNextNewParticleIdx++;
	return ret;
}

void GHParticleSystem::updateParticles(const GHTimeVal& time, const GHFrustum& frustum)
{
	// todo: take into acount being offscreen as part of this update time.
	//  we only update when drawing.
	float radiusInc, alphaMinus, velMod;
	GHPoint3 velAdd;
	float timeSinceLastUpdate = time.getTimePassed();
	float currTime = time.getTime();

	calcFrameUpdateInfo(timeSinceLastUpdate, radiusInc, alphaMinus, velMod, velAdd);
	for (unsigned int i = 0; i < mNumActiveParticles; ++i)
	{
		GHParticle& particle = *mParticles[i];
		updateParticle(particle, &frustum, velMod, alphaMinus, radiusInc, velAdd, timeSinceLastUpdate, currTime);
	}
}

void GHParticleSystem::calcFrameUpdateInfo(float timeSinceLastUpdate, float& radiusInc, float& alphaMinus, float& velMod, GHPoint3& velAdd)
{
	radiusInc = mDesc->get()->getRadiusGrowth() * timeSinceLastUpdate;
	alphaMinus = mDesc->get()->getAlphaMod() * timeSinceLastUpdate;
	velAdd = mDesc->get()->getGravity(); velAdd *= timeSinceLastUpdate;
	velMod = (1.0f - mDesc->get()->getFriction() * timeSinceLastUpdate);
	if (velMod < 0) velMod = 0;
}

void GHParticleSystem::updateParticle(GHParticle& particle, const GHFrustum* frustum,
	float velMod, float alphaMinus, float radiusInc,
	const GHPoint3& velAdd,
	float timeSinceLastUpdate, float currentTime)
{
	GHPoint3 pos;

	checkParticleLifetime(particle, currentTime);
	if (particle.isDead()) {
		return;
	}

	if (frustum) {
		particle.setDistFromCam(GHPlane::distFromPlane(frustum->getPlane(GHFrustum::CP_NEAR), pos));
	}

	/* djw note: spawning a bunch of particles in one frame.  this call makes them not update to a good state. not sure why it was added.
	if (particle.getStartTime() == currentTime) {
		return;
	}*/

	if (mDesc->get()->getAlphaMod() != 0) {
		particle.setAlpha(particle.getAlpha() - alphaMinus);
		if ((alphaMinus < 0 && particle.getAlpha() > 1) ||
			(alphaMinus > 0 && particle.getAlpha() < 0))
		{
			killParticle(particle);
			return;
		}
	}

	pos.setCoords(particle.getPosition().getCoords());
	for (int posIdx = 0; posIdx < 3; ++posIdx) {
		pos[posIdx] = pos[posIdx] + particle.getVelocity()[posIdx]*timeSinceLastUpdate;
	}
	particle.setPosition(pos);

	for (int velIdx = 0; velIdx < 3; ++velIdx) {
		particle.getVelocity()[velIdx] *= velMod;
		particle.getVelocity()[velIdx] += velAdd[velIdx];
	}

	//GHDebugMessage::outputString("radiusInc %f", radiusInc);
	float newRadius = particle.getRadius() + radiusInc;
	if (radiusInc < 0 && newRadius < 0) newRadius = 0;
	particle.setRadius(newRadius);

	if (particle.getRotationMod()) {
		particle.setCurrRotation(particle.getCurrRotation() + particle.getRotationMod() * timeSinceLastUpdate);
	}
}

void GHParticleSystem::checkParticleLifetime(GHParticle& particle, const float currentTime)
{
	if (particle.getEndTime() < currentTime) {
		killParticle(particle);
	}
}

void GHParticleSystem::killParticle(GHParticle& particle)
{
	particle.setDead(true);
}

void GHParticleSystem::updateBuffer(GHTransform billboardTrans)
{
	if (!mTargBuffer) return;

	GHVertexBuffer* targBuf = mTargBuffer->get();
	if (!targBuf) return;
	GHVBBlitterIndex* iblitter = (GHVBBlitterIndex*)targBuf->getBlitter()->get();
	GHVertexStream* stream = targBuf->getStream(0)->get();

	if (!mNumActiveParticles) {
		iblitter->setActiveIndices(0);
		return;
	}

	float* vbuf = (float*)stream->lockWriteBuffer();
	if (!vbuf) {
		return;
	}

	const GHVertexStreamFormat& vertDef = stream->getFormat();
	const GHVertexStreamFormat::ComponentEntry* posCompEntry = vertDef.getComponentEntry(GHVertexComponentShaderID::SI_POS);
	const GHVertexStreamFormat::ComponentEntry* uvCompEntry = vertDef.getComponentEntry(GHVertexComponentShaderID::SI_UV1);
	const GHVertexStreamFormat::ComponentEntry* normCompEntry = vertDef.getComponentEntry(GHVertexComponentShaderID::SI_NORMAL);
	const GHVertexStreamFormat::ComponentEntry* diffuseCompEntry = vertDef.getComponentEntry(GHVertexComponentShaderID::SI_DIFFUSE);
	assert(posCompEntry && uvCompEntry && normCompEntry && diffuseCompEntry);

	GHPoint3 nodePos;

	billboardTrans.setTranslate(GHPoint3(0, 0, 0));
	GHPoint3 normalizedPosOffsets[4];
	normalizedPosOffsets[0].setCoords(-1, -1, 0);
	normalizedPosOffsets[1].setCoords(1, -1, 0);
	normalizedPosOffsets[2].setCoords(-1, 1, 0);
	normalizedPosOffsets[3].setCoords(1, 1, 0);

	GHPoint2 uvs[4];
	uvs[0].setCoords(0, 1);
	uvs[1].setCoords(1, 1);
	uvs[2].setCoords(0, 0);
	uvs[3].setCoords(1, 0);

	GHPoint3 normals[4];
	normals[0].setCoords(-0.5, -0.5, 0.1f);
	normals[1].setCoords(0.5, -0.5, 0.1f);
	normals[2].setCoords(-0.5, 0.5, 0.1f);
	normals[3].setCoords(0.5, 0.5, 0.1f);

	GHPoint3 particlePosOffsets[4];
	if (mDesc->get()->getRadiusGrowth() == 1) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 2; ++j) {
				particlePosOffsets[i][j] = normalizedPosOffsets[i][j] * mDesc->get()->getParticleRadius();
			}
			billboardTrans.mult(particlePosOffsets[i], particlePosOffsets[i]);
		}
	}
	for (int i = 0; i < 4; ++i) {
		billboardTrans.mult(normals[i], normals[i]);
	}

	// count the number of live particles while doing this.
	int numLive = 0;
	for (unsigned int i = 0; i < mNumActiveParticles; ++i) {
		GHParticle& particle = *mParticles[i];
		if (particle.isDead()) {
			break;
		}
		numLive++;

		if (mDesc->get()->getRadiusGrowth() != 1) 
		{
			//GHDebugMessage::outputString("Scaling to %f", particle.getRadius());
			for (int j = 0; j < 4; ++j) {
				for (int k = 0; k < 2; ++k) {
					particlePosOffsets[j][k] = normalizedPosOffsets[j][k] * particle.getRadius();
				}
				particlePosOffsets[j][2] = 0;
				billboardTrans.mult(particlePosOffsets[j], particlePosOffsets[j]);
			}
		}

		GHPoint4 color;
		color.setCoords(1, 1, 1, particle.getAlpha());
		int32_t colorDword = GHColor::convertToDword(color);

		GHPoint2 dummyUV;

		const GHPoint3& particlePos = particle.getPosition();
		for (int whichVert = 0; whichVert < 4; ++whichVert)
		{
			// though you might be tempted to move this to a function, don't.
			// calling this as a function costs us .4ms per frame with only 10k particles.
			nodePos[0] = particlePos[0] + particlePosOffsets[whichVert][0] + mOffsetPos[0];
			nodePos[1] = particlePos[1] + particlePosOffsets[whichVert][1] + mOffsetPos[1];
			nodePos[2] = particlePos[2] + particlePosOffsets[whichVert][2] + mOffsetPos[2];

			float* posStart = vbuf + posCompEntry->mOffset;
			nodePos.fillBuffer(posStart);

			float* uvStart = vbuf + uvCompEntry->mOffset;
			if (!particle.getCurrRotation())
			{
				uvs[whichVert].fillBuffer(uvStart);
			}
			else
			{
				float rotCos = cos(particle.getCurrRotation());
				float rotSin = sin(particle.getCurrRotation());
				dummyUV[0] = rotCos * (uvs[whichVert][0] - 0.5f) - rotSin * (uvs[whichVert][1] - 0.5f) + 0.5f;
				dummyUV[1] = rotSin * (uvs[whichVert][0] - 0.5f) + rotCos * (uvs[whichVert][1] - 0.5f) + 0.5f;
				dummyUV.fillBuffer(uvStart);
			}

			float* normStart = vbuf + normCompEntry->mOffset;
			normals[whichVert].fillBuffer(normStart);

			float* diffuseStart = vbuf + diffuseCompEntry->mOffset;
			*((uint32_t*)diffuseStart) = colorDword;

			vbuf += vertDef.getVertexSize();
		}
	}

	stream->unlockWriteBuffer();

	mNumActiveParticles = numLive;
	iblitter->setActiveIndices(mNumActiveParticles * 2 * 3);
}

void GHParticleSystem::checkDead(const GHTimeVal& time)
{
	if (mIsDead) return;
	if (mDieTime == -1) {
		if (mDesc->get()->getLifetime() > 0) {
			mDieTime = time.getTime() + mDesc->get()->getLifetime();
		}
		return;
	}

	if (time.getTime() > mDieTime) {
		die();
	}
}

void GHParticleSystem::setTargBuffer(GHVertexBufferPtr* buffer)
{
	assert(buffer->get()->getStreams().size());
	assert(buffer->get()->getBlitter());
	assert(buffer->get()->getBlitter()->get());
	assert(buffer->get()->getBlitter()->get()->getType() == GHVBBlitter::BT_INDEX);
	GHVBBlitterIndex* ib = (GHVBBlitterIndex*)buffer->get()->getBlitter()->get();
	assert(ib->getNumIndices() >= mDesc->get()->getMaxParticles() * 2 * 3);

	GHRefCounted::changePointer((GHRefCounted*&)mTargBuffer, buffer);
}

