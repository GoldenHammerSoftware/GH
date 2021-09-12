// Copyright Golden Hammer Software
#pragma once

#include "Sim/GHAnimController.h"
#include "Sim/GHTransformNode.h"
#include "Render/GHParticleSystemDesc.h"
#include "Render/GHVertexBuffer.h"

class GHParticleSystem;
class GHGeometry;
class GHTimeVal;
class GHViewInfo;
class GHVBFactory;
class GHGeometryRenderable;

// updates a particle system when it is rendered.
// also responsible for creating an appropriate geometry.
class GHParticleAnimController : public GHAnimController
{
public:
	GHParticleAnimController(GHParticleSystemDescResource* desc, const GHTimeVal& time, 
		const GHViewInfo& viewInfo, GHVBFactory& vbFactory);
	~GHParticleAnimController(void);

	virtual void update(void);
	virtual GHAnimController* clone(void) const;

	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}

	virtual void setAnim(const GHIdentifier& id, float startTime = 0) {}
	virtual void setAnimBlend(const std::vector<AnimBlend>& blends) {}
	virtual AnimType getAnimType(void) const  { return GHAnimController::AT_UNKNOWN;  }

	GHGeometry& getGeo(void) { return *mGeo; }
	void attachRenderable(GHGeometryRenderable& renderable);
	GHParticleSystem& getParticle(void) { return *mParticle; }

	GHTransformNode* getTarget(void) const { return mTarget; }
	void setTarget(GHTransformNode* target) { mTarget = target; }

private:
	void createGeometry(void);
	GHVertexBufferPtr* createVB(const GHParticleSystemDescResource& desc);

private:
	const GHTimeVal& mTime;
	const GHViewInfo& mViewInfo;
	GHVBFactory& mVBFactory;
	GHParticleSystem* mParticle;
	// list of attached renderables, so we can detach to avoid double delete.
	std::vector<GHGeometryRenderable*> mRenderables;

	GHGeometry* mGeo;
	// the transform we pass to the particle system.
	GHTransformNode mTransformNode;
	// the transform that we follow around. (optional)
	GHTransformNode* mTarget;
};
