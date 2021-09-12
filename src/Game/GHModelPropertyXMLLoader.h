// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHResourceFactory;

// <modelProp file=kyamodel.xml/>
class GHModelPropertyXMLLoader : public GHXMLObjLoader
{
public:
    GHModelPropertyXMLLoader(GHResourceFactory& resourceCache);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHResourceFactory& mResourceCache;
};
