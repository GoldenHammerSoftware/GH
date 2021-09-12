// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHPlatform/GHOwnershipList.h"
#include "GHXMLSerializer.h"
#include "GHString/GHStringIdFactory.h"
#include <map>
#include "GHXMLCache.h"

class GHFileOpener;

class GHXMLObjFactory
{
public:
    GHXMLObjFactory(GHStringIdFactory& hashTable, 
                    const GHXMLSerializer& serializer);
    
    void* load(const char* filename, GHPropertyContainer* extraData=0) const;
    void* load(const GHXMLNode& node, GHPropertyContainer* extraData=0) const;

    void addLoader(GHXMLObjLoader* loader, 
                   unsigned int numNodeNames, 
                   const char* nodeName, ...);
    const GHXMLObjLoader* getLoader(const char* nodeName) const;

    // a way to trigger the caching outside of a recurse.
    void startCaching(void) const;
    void endCaching(void) const;
    
private:
    GHStringIdFactory& mIdFactory;
    const GHXMLSerializer& mSerializer;
    
    // list of resource loaders to delete.
    GHOwnershipList<GHXMLObjLoader> mLoaderDeleter;
    // map of node name to resource loader.
    std::map<GHIdentifier, GHXMLObjLoader*> mLoaders;

    // Cache of loaded nodes.
    // We increment a recurse count every time load(filename) is called,
    //  and decrement it at the end of the function.
    // While the recurse count is >0 we cache the nodes,
    //  and delete when the count hits 0 again.
    mutable GHXMLCache mNodeCache;
    mutable int mRecurseLoadCount;
};
