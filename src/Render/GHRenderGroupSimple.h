// Copyright Golden Hammer Software
#pragma once

#include "GHRenderGroup.h"
#include <vector>
#include "GHRenderPassMembership.h"
#include <map>

// minimal implementation of the GHRenderGroup interface
class GHRenderGroupSimple : public GHRenderGroup
{
public:
    GHRenderGroupSimple(void);
    
    virtual void collect(GHRenderPass& pass, const GHFrustum* frustum);
    
    virtual void addRenderable(GHRenderable& renderable, const GHIdentifier& type);
    virtual void removeRenderable(GHRenderable& renderable, const GHIdentifier& type);

    GHRenderPassMembership& getPassMembership(void) { return mPassMembership; }
    
	void setFarClip(const GHIdentifier& passId, float val);
    
private:
    std::vector<GHRenderable*> mRenderables;
    GHRenderPassMembership mPassMembership;
    // if 0 or not present then don't override the frustum.
    std::map<GHIdentifier, float> mFarClip;
};
