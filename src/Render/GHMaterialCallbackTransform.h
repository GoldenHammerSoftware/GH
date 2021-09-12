// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallback.h"

class GHViewInfo;
class GHMaterialParamHandle;

// A callback to set world transforms on a material
class GHMaterialCallbackTransform : public GHMaterialCallback
{
public:
    GHMaterialCallbackTransform(GHMaterialParamHandle* modelViewProjHandle,
                                GHMaterialParamHandle* modelToWorldHandle,
                                GHMaterialParamHandle* worldToModelHandle,
                                GHMaterialParamHandle* eyePosModelHandle,
								GHMaterialParamHandle* eyeDirModelHandle);
    virtual ~GHMaterialCallbackTransform(void);
    
    virtual void apply(const void* data);
    
private:
    GHMaterialParamHandle* mModelViewProjHandle;
    GHMaterialParamHandle* mModelToWorldHandle;
    GHMaterialParamHandle* mWorldToModelHandle;
    GHMaterialParamHandle* mEyePosModelHandle;
	GHMaterialParamHandle* mEyeDirModelHandle;
};
