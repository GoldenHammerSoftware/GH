// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHResourceFactory;

// class to load a clone of a model from the resource cache from within xml.
// example: <cacheModel file=kyahead.xml/>
class GHCacheModelLoader : public GHXMLObjLoader
{
public:
    GHCacheModelLoader(GHResourceFactory& cache);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHResourceFactory& mCache;
};
