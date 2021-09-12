// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHString.h"

// util class to pass arguments to an md3 load
class GHMD3LoaderConfig
{
public:
    GHMD3LoaderConfig(void) : mLoadingScale(0) {}
    
    // an optional tag to center all other tags around.
    GHString mCenterTagName;
    // a value to override the coords in the md3 by a scale.
    float mLoadingScale;
};
