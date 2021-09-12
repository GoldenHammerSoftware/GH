// Copyright Golden Hammer Software
#include "GHSceneRenderer.h"
#include "GHRenderPass.h"
#include "GHRenderable.h"
#include "GHRenderDevice.h"
#include <algorithm>
#include "GHPlatform/GHStlUtil.h"
#include "GHUtils/GHProfiler.h"
#include "GHRenderStats.h"
#include "GHHMDRenderDevice.h"
#include "GHRenderTarget.h"

GHSceneRenderer::GHSceneRenderer(GHRenderDevice& renderDevice, GHPropertyContainer& props)
: mRenderDevice(renderDevice)
, mProps(props)
{
}

GHSceneRenderer::~GHSceneRenderer(void)
{
    GHStlUtil::deletePointerList(mPasses);
}

void GHSceneRenderer::update(void)
{
	GHRenderStats::beginFrame();
    GHPROFILEBEGIN("GHSceneRenderer::update")

    GHPROFILEBEGIN("GHSceneRenderer::update - RenderBegin")
    if (!mRenderDevice.beginFrame()) {
        return;
    }
	bool clearMainBuffers = (mHMDRenderType != GHRenderPass::HMDRT_HMDONLY);
	mRenderDevice.beginRenderPass(0, GHPoint4(0, 0, 0, 0), clearMainBuffers);
    GHPROFILEEND("GHSceneRenderer::update - RenderBegin")
    
	GHPROFILEBEGIN("GHSceneRenderer::update - RenderCollect")
	for (size_t passId = 0; passId < mPasses.size(); ++passId)
    {
        mPasses[passId]->prepareForCollect();
    }
    
    for (size_t passId = 0; passId < mPasses.size(); ++passId)
    {
        mRenderGroup.collect(*mPasses[passId], &mPasses[passId]->getViewInfo(GHViewInfo::EYE_MONOCLE).getFrustum());
    }
	GHPROFILEEND("GHSceneRenderer::update - RenderCollect")

	GHPROFILEBEGIN("GHSceneRenderer::update - FlushPasses")
	GHRenderTarget* hmdTarget = 0;
	GHRenderTarget* hmdLeftTarget = 0;
	GHRenderTarget* hmdRightTarget = 0;
	if (mRenderDevice.getHMDDevice() && mRenderDevice.getHMDDevice()->isActive())
	{
		hmdTarget = mRenderDevice.getHMDDevice()->getRenderTarget(GHHMDRenderDevice::RTT_BOTH);
		hmdLeftTarget = mRenderDevice.getHMDDevice()->getRenderTarget(GHHMDRenderDevice::RTT_LEFT);
		hmdRightTarget = mRenderDevice.getHMDDevice()->getRenderTarget(GHHMDRenderDevice::RTT_RIGHT);
	}

	// minimize the number of render target switches.
	// we're assuming the number of passes is relatively small, and the cost of a render target swap is relatively high.

	// first the main view.
	for (size_t passId = 0; passId < mPasses.size(); ++passId)
	{
		GHRenderPass* currPass = mPasses[passId];
		if (currPass->getHMDRenderType() != GHRenderPass::HMDRT_HMDONLY)
		{
			// the idea here is to disable the main screen draw without disabling any offscreen passes.
			if (mHMDRenderType != GHRenderPass::HMDRT_HMDONLY || mPasses[passId]->getRenderTarget())
			{
				currPass->draw();
			}
		}
	}
	if (mHMDRenderType != GHRenderPass::HMDRT_MAINONLY)
	{
		GHPROFILESCOPE("GHSceneRenderer::update stereoPasses", GHString::CHT_COPY);

		// then the stereo views.
		// doing each view independently to avoid repeatedly swapping the render target.
		renderStereoPasses(hmdTarget, GHViewInfo::EYE_MONOCLE);
		renderStereoPasses(hmdLeftTarget, GHViewInfo::EYE_LEFT);
		renderStereoPasses(hmdRightTarget, GHViewInfo::EYE_RIGHT);
	}
	for (size_t passId = 0; passId < mPasses.size(); ++passId)
	{
		GHRenderPass* currPass = mPasses[passId];
		currPass->clear();
	}

    GHPROFILEEND("GHSceneRenderer::update - FlushPasses")
    
    GHPROFILEBEGIN("GHSceneRenderer::update - endFrame")
	mRenderDevice.endRenderPass();
	mRenderDevice.endFrame();
    GHPROFILEEND("GHSceneRenderer::update - endFrame")
    
    GHPROFILEEND("GHSceneRenderer::update")
	GHRenderStats::endFrame(mProps);
}

void GHSceneRenderer::drawRenderPass(GHRenderPass& pass)
{
	if (!mRenderDevice.beginFrame()) {
		return;
	}

	pass.prepareForCollect();
	mRenderGroup.collect(pass, &pass.getViewInfo(GHViewInfo::EYE_MONOCLE).getFrustum());
	pass.draw();
	pass.clear();

	mRenderDevice.endFrame();
}

void GHSceneRenderer::renderStereoPasses(GHRenderTarget* hmdTarget, GHViewInfo::EyeIndex eyeIndex)
{
	if (!hmdTarget)
	{
		return;
	}
	bool usedAPass = false;
	for (size_t passId = 0; passId < mPasses.size(); ++passId)
	{
		GHRenderPass* currPass = mPasses[passId];

		if (currPass->getHMDRenderType() == GHRenderPass::HMDRT_MAINONLY)
		{
			// We could store the passes in a secondary list that does not include mainonly, 
			// but it should be a short enough list to not worry about.
			continue;
		}

		if (!usedAPass)
		{
			// Apply the render target exactly once per eye if we found a pass in that eye.
			hmdTarget->apply();
			usedAPass = true;
		}
		
		currPass->getRenderList().setStereoRenderType(eyeIndex);
		currPass->draw();
	}
	if (usedAPass)
	{
		hmdTarget->remove();
	}
}

void GHSceneRenderer::addRenderPass(GHRenderPass* pass)
{
    mPasses.push_back(pass);
}

void GHSceneRenderer::removeRenderPass(GHRenderPass* pass)
{
    std::vector<GHRenderPass*>::iterator findIter;
    findIter = std::find(mPasses.begin(), mPasses.end(), pass);
    if (findIter != mPasses.end()) mPasses.erase(findIter);
}

