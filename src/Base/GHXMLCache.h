// Copyright Golden Hammer Software
#pragma once

#include <map>
#include "GHString/GHIdentifier.h"

class GHXMLNode;
class GHStringIdFactory;
class GHXMLSerializer;

// A class for loading and caching xml nodes.
class GHXMLCache
{
public:
    GHXMLCache(GHStringIdFactory& hashTable,
               const GHXMLSerializer& serializer);
    ~GHXMLCache(void);
    
    GHXMLNode* getNode(const char* fileName);
    void clearCache(void);
    
private:
    GHStringIdFactory& mIdFactory;
    const GHXMLSerializer& mSerializer;
    std::map<GHIdentifier, GHXMLNode*> mNodeCache;
};
