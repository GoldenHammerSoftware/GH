// Copyright Golden Hammer Software
#include "GHResourceFactory.h"
#include "GHString/GHStringIdFactory.h"
#include "GHResourceCache.h"
#include "GHResource.h"
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include "GHPlatform/GHDebugMessage.h"

GHResourceFactory::GHResourceFactory(GHResourceCache& resCache,
                                     const GHStringIdFactory& hashTable)
: mResourceCache(resCache)
, mIdFactory(hashTable)
{
}

void GHResourceFactory::addLoader(GHResourceLoader* loader, 
                                  unsigned int numExtensions,
                                  const char* extension, ...)
{
    mLoaderDeleter.push_back(loader);
    
    va_list extList;
    va_start(extList, extension);
    const char* extStr = extension;
    for (size_t i = 0; i < numExtensions; ++i)
    {
        GHIdentifier extHash = mIdFactory.generateHash(extStr);
        mLoaders[extHash] = loader;
        extStr = va_arg(extList, const char*);
    }
    va_end(extList);
}

GHResourceLoader* GHResourceFactory::getLoader(const char* extension) const
{
    GHIdentifier extHash = mIdFactory.generateHash(extension);
    std::map<GHIdentifier, GHResourceLoader*>::const_iterator loaderFinder = mLoaders.find(extHash);
    if (loaderFinder == mLoaders.end()) return 0;
    return loaderFinder->second;
}

GHResource* GHResourceFactory::getCacheResource(const char* fileName, GHPropertyContainer* extraData)
{
    GHIdentifier fileId = mIdFactory.generateHash(fileName);
    
    // first see if we already have this loaded.
    GHResource* res = mResourceCache.getResource(fileId);
    if (res) {
        return res;
    }
    
    res = getUniqueResource(fileName, extraData);
    if (!res) {
		//GHDebugMessage::outputString("Failed to load cache resource %s", fileName);
        return 0;
    }
    mResourceCache.storeResource(fileId, res);
    return res;
}

GHResource* GHResourceFactory::getUniqueResource(const char* fileName, GHPropertyContainer* extraData)
{
    const char* extStr = ::strrchr(fileName,'.');
    GHIdentifier extHash = mIdFactory.generateHash(extStr);
    std::map<GHIdentifier, GHResourceLoader*>::iterator loaderFinder = mLoaders.find(extHash);
    if (loaderFinder == mLoaders.end()) {
		//GHDebugMessage::outputString("Failed to find loader for %s", fileName);
		return 0;
	}
    
    GHResource* ret = loaderFinder->second->loadFile(fileName, extraData);
    if (ret) {
        GHIdentifier fileId = mIdFactory.generateHash(fileName);
        ret->assignId(fileId, 0);
    }
	else {
		//GHDebugMessage::outputString("Failed to load unique resource %s", fileName);
	}
    return ret;
}
