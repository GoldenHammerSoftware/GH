// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallbackFactory.h"

class GHMaterialCallbackFactoryTransform : public GHMaterialCallbackFactory
{
public:
    virtual void createCallbacks(GHMaterial& mat) const;
};
