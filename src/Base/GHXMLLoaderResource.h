// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include <assert.h>

class GHResourceFactory;

// an XML loader for redirecting to the resource cache
// format <resource file="blah.jpg"/>
class GHXMLLoaderResource : public GHXMLObjLoader
{
public:
    GHXMLLoaderResource(GHResourceFactory& cache);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
               
private:
    GHResourceFactory& mCache;
};
