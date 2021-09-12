// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHOwnershipList.h"
#include "GHResourceLoader.h"
#include <map>
#include "GHString/GHIdentifier.h"

class GHResourceCache;
class GHResource;
class GHStringIdFactory;

class GHResourceFactory
{
public:
    GHResourceFactory(GHResourceCache& resCache,
                      const GHStringIdFactory& hashTable);
    
    // Get the cache copy of a resource if loaded,
    // otherwise load it and also store it for later.
    GHResource* getCacheResource(const char* fileName, GHPropertyContainer* extraData=0);
    // Load a unique copy from file of a resource.
    GHResource* getUniqueResource(const char* fileName, GHPropertyContainer* extraData=0);

    void addLoader(GHResourceLoader* loader, 
                   unsigned int numExtensions, 
                   const char* extension, ...);
	// The GHResourceLoader interface is not const, so returning a const* wouldn't be useful.
    GHResourceLoader* getLoader(const char* extension) const;

private:
    GHResourceCache& mResourceCache;
    const GHStringIdFactory& mIdFactory;
    // list of resource loaders to delete.
    GHOwnershipList<GHResourceLoader> mLoaderDeleter;
    // map of extension string (".jpg") to resource loader.
    std::map<GHIdentifier, GHResourceLoader*> mLoaders;
};
