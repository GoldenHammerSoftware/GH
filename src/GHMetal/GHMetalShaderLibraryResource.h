#pragma once

#include "GHUtils/GHResource.h"

// Wrapper for loading .metal files from the resource manager.
class GHMetalShaderLibraryResource : public GHResource
{
public:
    id<MTLLibrary> mLibrary;
};
