// Copyright Golden Hammer Software
#include "GHParticleAnimController.h"
#include "GHParticleSystem.h"
#include "GHGeometry.h"
#include "GHVBFactory.h"
#include "GHIndexBufferUtil.h"
#include "GHGeometryRenderable.h"

GHParticleAnimController::GHParticleAnimController(GHParticleSystemDescResource* desc, const GHTimeVal& time,
	const GHViewInfo& viewInfo, GHVBFactory& vbFactory)
: mParticle(0)
, mTime(time)
, mVBFactory(vbFactory)
, mViewInfo(viewInfo)
, mGeo(0)
, mTarget(0)
{
	mParticle = new GHParticleSystem(desc);
	mTransformNode.getLocalTransform().becomeIdentity();
	createGeometry();
}

GHParticleAnimController::~GHParticleAnimController(void)
{
	for (size_t i = 0; i < mRenderables.size(); ++i) {
		mRenderables[i]->removeGeometry(mGeo);
	}
	delete mParticle;
	delete mGeo;
}

void GHParticleAnimController::update(void)
{
	if (mTarget) {
		GHPoint3 trans(0, 0, 0);
		mTarget->getTransform().getTranslate(trans);
		if (!mParticle->getDesc()->get()->getIsWorldSpace()) 
		{
			mParticle->setOffsetPos(trans);
		}
		else 
		{
			mParticle->setRootPos(trans);
		}
	}
	mParticle->update(mTime, mViewInfo);
}

GHAnimController* GHParticleAnimController::clone(void) const
{
	GHParticleAnimController* ret = new GHParticleAnimController(mParticle->getDesc(), mTime, mViewInfo, mVBFactory);
	return ret;
}

void GHParticleAnimController::createGeometry(void)
{
	mGeo = new GHGeometry();
	mGeo->setTransform(&mTransformNode);
	mGeo->setMaterial(mParticle->getDesc()->get()->getMaterial());
	mGeo->setVB(createVB(*mParticle->getDesc()));
	mParticle->setTargBuffer(mGeo->getVB());
}

GHVertexBufferPtr* GHParticleAnimController::createVB(const GHParticleSystemDescResource& desc)
{
	const int numVerts = mParticle->getDesc()->get()->getMaxParticles() * 4;
	const int numIndices = mParticle->getDesc()->get()->getMaxParticles() * 2 * 3;

	GHVBDescription vbdesc(numVerts, numIndices, GHVBBlitter::BT_INDEX);
	GHVBDescription::StreamDescription streamDesc;
	streamDesc.mIsShared = false;
	streamDesc.mUsage = GHVBUsage::DYNAMIC;
	streamDesc.mComps.push_back(GHVertexComponent::AP_POS);
	streamDesc.mComps.push_back(GHVertexComponent::AP_NORMAL);
	streamDesc.mComps.push_back(GHVertexComponent::AP_UV1);
	streamDesc.mComps.push_back(GHVertexComponent::AP_DIFFUSE);
	vbdesc.addStreamDescription(streamDesc);
	GHVertexBuffer* ret = mVBFactory.createVB(vbdesc);
	assert(ret);
	assert(ret->getBlitter());
	assert(ret->getBlitter()->get()->getType() == GHVBBlitter::BT_INDEX);

	GHIndexBufferUtil::fillIndexBufferForQuads(*((GHVBBlitterIndex*)ret->getBlitter()->get()), numVerts / 4);
	return new GHVertexBufferPtr(ret);
}

void GHParticleAnimController::attachRenderable(GHGeometryRenderable& renderable)
{
	renderable.addGeometry(mGeo);
	mRenderables.push_back(&renderable);
}
