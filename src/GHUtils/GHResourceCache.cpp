// Copyright Golden Hammer Software
#include "GHResourceCache.h"
#include "stdarg.h"
#include "GHResource.h"
#include "assert.h"
#include "GHPlatform/GHDebugMessage.h"

GHResourceCache::~GHResourceCache(void)
{
    // cache should be empty at this point.
    garbageCollect();
    size_t numResources = mResources.size();
    if (numResources > 0)
    {
        ResourceMap::iterator resIter;
        for (resIter = mResources.begin(); resIter != mResources.end(); ++resIter)
        {
            GHResource* res = resIter->second;
#ifdef DEBUG_IDENTIFIERS
            GHDebugMessage::outputString("Leaking %s %d", res->getResourceId().getString(), res->getRefCount());
#endif
            res = 0; // dummy use of res var.
        }
        assert(numResources == 0);
    }
}

void GHResourceCache::garbageCollect(void)
{
    ResourceMap::iterator resIter;
    for (resIter = mResources.begin(); resIter != mResources.end(); ++resIter)
    {
        GHResource* res = resIter->second;
        if (res->getRefCount() == 0)
        {
            ResourceMap::iterator eraseIter = resIter;
            resIter--;
            eraseIter->second->release();
            mResources.erase(eraseIter);
        }
    }
}

GHResource* GHResourceCache::getResource(const GHIdentifier& id)
{
    ResourceMap::iterator findIter;
    findIter = mResources.find(id);
    if (findIter != mResources.end())
    {
        return findIter->second;
    }
    return 0;
}

void GHResourceCache::storeResource(const GHIdentifier& id, GHResource* resource)
{
    mResources[id] = resource;
    resource->assignId(id, this);
}

void GHResourceCache::removeResource(const GHIdentifier& id)
{
    ResourceMap::iterator findIter;
    findIter = mResources.find(id);
    if (findIter != mResources.end())
    {
        mResources.erase(findIter);
    }
}

