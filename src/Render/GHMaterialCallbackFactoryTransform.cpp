// Copyright Golden Hammer Software
#include "GHMaterialCallbackFactoryTransform.h"
#include "GHMaterialCallbackTransform.h"
#include "GHMaterial.h"

void GHMaterialCallbackFactoryTransform::createCallbacks(GHMaterial& mat) const
{
    GHMaterialParamHandle* modelViewProjHandle = mat.getParamHandle("ModelViewProj");
    GHMaterialParamHandle* modelToWorldHandle = mat.getParamHandle("ModelToWorld");
    GHMaterialParamHandle* worldToModelHandle = mat.getParamHandle("WorldToModel");
    GHMaterialParamHandle* eyePosModelHandle = mat.getParamHandle("EyePosModel");
	GHMaterialParamHandle* eyeDirModelHandle = mat.getParamHandle("EyeDirModel");
    
    if (modelViewProjHandle || modelToWorldHandle || worldToModelHandle || eyePosModelHandle || eyeDirModelHandle)
    {
        GHMaterialCallbackTransform* cb = new GHMaterialCallbackTransform(modelViewProjHandle, modelToWorldHandle, worldToModelHandle, eyePosModelHandle, eyeDirModelHandle);
        mat.addCallback(GHMaterialCallbackType::CT_PERTRANS, cb);
    }
}
