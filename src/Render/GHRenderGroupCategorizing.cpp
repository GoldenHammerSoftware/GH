// Copyright Golden Hammer Software
#include "GHRenderGroupCategorizing.h"

GHRenderGroupCategorizing::GHRenderGroupCategorizing(void)
{
}

GHRenderGroupCategorizing::~GHRenderGroupCategorizing(void)
{
    std::map<GHIdentifier, GHRenderGroup*>::const_iterator iter;
    for (iter = mGroups.begin(); iter != mGroups.end(); ++iter)
    {
        delete iter->second;
    }
}

void GHRenderGroupCategorizing::collect(GHRenderPass& pass, const GHFrustum* frustum)
{
    mDefaultGroup.collect(pass, frustum);
    std::map<GHIdentifier, GHRenderGroup*>::const_iterator iter;
    for (iter = mGroups.begin(); iter != mGroups.end(); ++iter)
    {
        iter->second->collect(pass, frustum);
    }
}

void GHRenderGroupCategorizing::addRenderable(GHRenderable& renderable, const GHIdentifier& type)
{
    std::map<GHIdentifier, GHRenderGroup*>::iterator iter;
    iter = mGroups.find(type);
    if (iter != mGroups.end())
    {
        iter->second->addRenderable(renderable, type);
    }
    else
    {
        mDefaultGroup.addRenderable(renderable, type);
    }
}

void GHRenderGroupCategorizing::removeRenderable(GHRenderable& renderable, const GHIdentifier& type)
{
    std::map<GHIdentifier, GHRenderGroup*>::iterator iter;
    iter = mGroups.find(type);
    if (iter != mGroups.end())
    {
        iter->second->removeRenderable(renderable, type);
    }
    else
    {
        mDefaultGroup.removeRenderable(renderable, type);
    }
}

void GHRenderGroupCategorizing::addRenderGroup(const GHIdentifier& type, GHRenderGroup* group)
{
    std::map<GHIdentifier, GHRenderGroup*>::iterator iter;
    iter = mGroups.find(type);
    if (iter != mGroups.end())
    {
        delete iter->second;
    }
    mGroups[type] = group;
}

