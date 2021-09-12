#include "GHResource.h"
#include "GHResourceCache.h"

GHResource::GHResource(void)
: GHRefCounted()
, mResourceCache(0)
, mResourceId(0)
{
}

GHResource::~GHResource(void)
{
    if (mResourceCache)
    {
        mResourceCache->removeResource(mResourceId);
    }
}

void GHResource::assignId(GHIdentifier resourceId, GHResourceCache* cache)
{
    mResourceCache = cache;
    mResourceId = resourceId;
}
