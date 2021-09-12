#include "GHXMLLoaderResource.h"
#include "GHXMLNode.h"
#include "GHUtils/GHResourceFactory.h"

GHXMLLoaderResource::GHXMLLoaderResource(GHResourceFactory& cache)
: mCache(cache)
{
}

void* GHXMLLoaderResource::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* resName = node.getAttribute("file");
    return mCache.getCacheResource(resName);
}
