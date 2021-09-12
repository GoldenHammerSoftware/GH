#pragma once

// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallbackFactory.h"

class GHGUIMaterialCallbackFactoryTransform : public GHMaterialCallbackFactory
{
public:
	virtual void createCallbacks(GHMaterial& mat) const;
};
