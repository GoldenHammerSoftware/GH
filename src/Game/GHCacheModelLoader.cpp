// Copyright Golden Hammer Software
#include "GHCacheModelLoader.h"
#include "GHXMLNode.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHModel.h"

GHCacheModelLoader::GHCacheModelLoader(GHResourceFactory& cache)
: mCache(cache)
{
}

void* GHCacheModelLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHModel* ret = (GHModel*)mCache.getCacheResource(node.getAttribute("file"));
    if (ret) ret = ret->clone();
    return ret;
}
