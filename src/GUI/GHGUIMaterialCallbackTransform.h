// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallback.h"

class GHMaterialParamHandle;

// A callback to transform guis into the world, position them, and put them back on the screen.
class GHGUIMaterialCallbackTransform : public GHMaterialCallback
{
public:
	GHGUIMaterialCallbackTransform(GHMaterialParamHandle* guiInWorldHandle);
	virtual ~GHGUIMaterialCallbackTransform(void);

	virtual void apply(const void* data);

private:
	GHMaterialParamHandle* mGUIInWorldHandle;
};

