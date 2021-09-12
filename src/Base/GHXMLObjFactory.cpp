// Copyright Golden Hammer Software
#include "GHXMLObjFactory.h"
#include "GHXMLNode.h"
#include "GHXMLObjLoader.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/GHDebugMessage.h"
#include <stdarg.h>
#include "GHUtils/GHProfiler.h"

GHXMLObjFactory::GHXMLObjFactory(GHStringIdFactory& hashTable, 
                                 const GHXMLSerializer& serializer)
: mIdFactory(hashTable)
, mSerializer(serializer)
, mRecurseLoadCount(0)
, mNodeCache(hashTable, serializer)
{
}

void* GHXMLObjFactory::load(const char* filename, GHPropertyContainer* extraData) const
{
    GHPROFILESCOPE("GHXMLObjFactory::load", GHString::CHT_REFERENCE)
    
    startCaching();
    GHXMLNode* node = mNodeCache.getNode(filename);
	if (!node) {
		GHDebugMessage::outputString("Failed to open %s", filename);
		return 0;
	}
    void* ret = load(*node, extraData);
    endCaching();

	return ret;
}

void* GHXMLObjFactory::load(const GHXMLNode& node, GHPropertyContainer* extraData) const
{
    const char* nodeName = node.getName();
    const GHXMLObjLoader* loader = getLoader(nodeName);
    if (!loader) {
        GHDebugMessage::outputString("Could not find loader for %s", nodeName);
        return 0;
    }
    if (extraData) {
        return loader->load(node, *extraData, *this);
    }
    else {
        GHPropertyContainer argProps;
        return loader->load(node, argProps, *this);
    }
}

void GHXMLObjFactory::addLoader(GHXMLObjLoader* loader, 
                                unsigned int numNodeNames, 
                                const char* nodeName, ...)
{
    mLoaderDeleter.push_back(loader);
    
    va_list nameList;
    va_start(nameList, nodeName);
    const char* nameStr = nodeName;
    for (size_t i = 0; i < numNodeNames; ++i)
    {
        GHIdentifier nameHash = mIdFactory.generateHash(nameStr);
        mLoaders[nameHash] = loader;
        nameStr = va_arg(nameList, const char*);
    }
    va_end(nameList);
}

const GHXMLObjLoader* GHXMLObjFactory::getLoader(const char* nodeName) const
{
    GHIdentifier nameHash = mIdFactory.generateHash(nodeName);
    std::map<GHIdentifier, GHXMLObjLoader*>::const_iterator loaderFinder = mLoaders.find(nameHash);
    if (loaderFinder == mLoaders.end()) return 0;
    return loaderFinder->second;
}

void GHXMLObjFactory::startCaching(void) const
{
    mRecurseLoadCount++;
}
void GHXMLObjFactory::endCaching(void) const
{
    mRecurseLoadCount--;
    if (mRecurseLoadCount == 0)
    {
        mNodeCache.clearCache();
    }
}
