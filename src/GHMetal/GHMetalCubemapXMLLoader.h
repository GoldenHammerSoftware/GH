#pragma once

#include "GHXMLObjLoader.h"

class GHResourceFactory;
class GHMetalRenderDevice;

class GHMetalCubemapXMLLoader : public GHXMLObjLoader
{
public:
    GHMetalCubemapXMLLoader(GHResourceFactory& resourceFactory, GHMetalRenderDevice& device);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE

private:
    GHResourceFactory& mResourceFactory;
    GHMetalRenderDevice& mDevice;
};

