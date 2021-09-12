// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include <vector>
#include "GHRenderGroupCategorizing.h"
#include "GHRenderPass.h"

class GHPropertyContainer;
class GHRenderable;
class GHRenderDevice;

// a class to manage a set of renderable things and draw them in passes.
class GHSceneRenderer : public GHController
{
public:
    GHSceneRenderer(GHRenderDevice& renderDevice, GHPropertyContainer& props);
    ~GHSceneRenderer(void);
    
    virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}

    void addRenderPass(GHRenderPass* pass);
    void removeRenderPass(GHRenderPass* pass);

    void drawRenderPass(GHRenderPass& pass);
    
    GHRenderGroupCategorizing& getRenderGroup(void) { return mRenderGroup; }
    
	void setHMDRenderType(GHRenderPass::HMDRenderType renderType) { mHMDRenderType = renderType; }

private:
	void renderStereoPasses(GHRenderTarget* hmdTarget, GHViewInfo::EyeIndex eyeIndex);

private:
    GHRenderGroupCategorizing mRenderGroup;
    GHRenderDevice& mRenderDevice;
	GHPropertyContainer& mProps;
    // we own the passes
    std::vector<GHRenderPass*> mPasses;

	// flag to modify which passes get drawn.
	GHRenderPass::HMDRenderType mHMDRenderType{ GHRenderPass::HMDRT_BOTH };
};
