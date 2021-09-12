// Copyright Golden Hammer Software
#include "GHGUIMaterialCallbackFactoryTransform.h"
#include "GHGUIMaterialCallbackTransform.h"
#include "GHMaterial.h"

void GHGUIMaterialCallbackFactoryTransform::createCallbacks(GHMaterial& mat) const
{
	GHMaterialParamHandle* guiInWorldHandle = mat.getParamHandle("GUIInWorldTransform");

	if (guiInWorldHandle)
	{
		GHGUIMaterialCallbackTransform* cb = new GHGUIMaterialCallbackTransform(guiInWorldHandle);
		mat.addCallback(GHMaterialCallbackType::CT_PERENT, cb);
	}
}
