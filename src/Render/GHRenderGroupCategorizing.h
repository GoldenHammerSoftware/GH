// Copyright Golden Hammer Software
#pragma once

#include "GHRenderGroup.h"
#include "GHRenderGroupSimple.h"
#include <map>

// a GHRenderGroup that wraps other GHRenderGroups by category
// any type not given a specific subgroup is put in the default category.
class GHRenderGroupCategorizing : public GHRenderGroup
{
public:
    GHRenderGroupCategorizing(void);
    ~GHRenderGroupCategorizing(void);
    
    virtual void collect(GHRenderPass& pass, const GHFrustum* frustum);
    
    virtual void addRenderable(GHRenderable& renderable, const GHIdentifier& type);
    virtual void removeRenderable(GHRenderable& renderable, const GHIdentifier& type);
    
    // assign a render group to a category and take ownership.
    void addRenderGroup(const GHIdentifier& type, GHRenderGroup* group);
    
	GHRenderGroupSimple& getDefaultRenderGroup(void) { return mDefaultGroup; }

private:
    GHRenderGroupSimple mDefaultGroup;
    std::map<GHIdentifier, GHRenderGroup*> mGroups;
};
