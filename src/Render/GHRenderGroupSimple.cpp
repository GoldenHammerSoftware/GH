// Copyright Golden Hammer Software
#include "GHRenderGroupSimple.h"
#include <algorithm>
#include <stddef.h>
#include "GHRenderPass.h"

GHRenderGroupSimple::GHRenderGroupSimple(void)
{
    mPassMembership.setEmptySupportsAll(true);
}

void GHRenderGroupSimple::collect(GHRenderPass& pass, const GHFrustum* frustum)
{
    if (!mPassMembership.supportsPass(pass.getIdentifier()))
    {
        return;
    }
    
	float farClip = 0;
	std::map<GHIdentifier, float>::const_iterator clipIter = mFarClip.find(pass.getIdentifier());
	if (clipIter != mFarClip.end()) {
		farClip = clipIter->second;
	}

    const GHFrustum* cullFrustum = frustum;
    if (frustum && farClip)
    {
        GHFrustum* newFrustum = new GHFrustum(*frustum);
        newFrustum->setFarDist(farClip);
        cullFrustum = newFrustum;
    }
    
    for (size_t renderableId = 0; renderableId < mRenderables.size(); ++renderableId)
    {
        GHRenderable* currRenderable = mRenderables[renderableId];
        if (mDrawOrder) currRenderable->setDrawOrder(mDrawOrder);
        currRenderable->collect(pass, cullFrustum);
    }
    
    if (frustum && farClip)
    {
        delete cullFrustum;
    }
}

void GHRenderGroupSimple::addRenderable(GHRenderable& renderable, const GHIdentifier& type)
{
    mRenderables.push_back(&renderable);
}

void GHRenderGroupSimple::removeRenderable(GHRenderable& renderable, const GHIdentifier& type)
{
    std::vector<GHRenderable*>::iterator findIter;
    findIter = std::find(mRenderables.begin(), mRenderables.end(), &renderable);
    if (findIter != mRenderables.end()) mRenderables.erase(findIter);
}

void GHRenderGroupSimple::setFarClip(const GHIdentifier& passId, float val)
{
	mFarClip[passId] = val;
}

