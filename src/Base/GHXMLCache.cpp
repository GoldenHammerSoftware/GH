// Copyright Golden Hammer Software
#include "GHXMLCache.h"
#include "GHString/GHStringIdFactory.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"

GHXMLCache::GHXMLCache(GHStringIdFactory& hashTable,
                       const GHXMLSerializer& serializer)
: mIdFactory(hashTable)
, mSerializer(serializer)
{
}

GHXMLCache::~GHXMLCache(void)
{
    clearCache();
}

GHXMLNode* GHXMLCache::getNode(const char* fileName)
{
    GHIdentifier mapId = mIdFactory.generateHash(fileName);
    
    GHXMLNode* node = 0;
    bool isCacheNode = false;
    std::map<GHIdentifier, GHXMLNode*>::iterator findIter;
    findIter = mNodeCache.find(mapId);
    if (findIter != mNodeCache.end())
    {
        node = findIter->second;
        isCacheNode = true;
    }
    else {
        node = mSerializer.loadXMLFile(fileName);
    }
    if (!node) return 0;
    
    if (!isCacheNode) {
        mNodeCache.insert(std::pair<GHIdentifier, GHXMLNode*>(mapId, node));
    }
    return node;
}

void GHXMLCache::clearCache(void)
{
    std::map<GHIdentifier, GHXMLNode*>::iterator clearIter;
    for (clearIter = mNodeCache.begin(); clearIter != mNodeCache.end(); ++clearIter)
    {
        delete clearIter->second;
    }
    mNodeCache.clear();
}
