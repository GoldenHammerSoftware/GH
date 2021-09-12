// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallback.h"

class GHLightMgr;
class GHMaterialParamHandle;

// A callback for applying one light for a material.
class GHLightMaterialCallback : public GHMaterialCallback
{
public:
    GHLightMaterialCallback(const GHLightMgr& lightMgr, int lightIndex,
                            GHMaterialParamHandle* dirHandle,
							GHMaterialParamHandle* lumHandle,
                            GHMaterialParamHandle* ambientParamHandle);
    virtual ~GHLightMaterialCallback(void);
    
    // data is transform
    virtual void apply(const void* data);
    
private:
    const GHLightMgr& mLightMgr;
    int mLightIndex;
    GHMaterialParamHandle* mDirHandle;
	GHMaterialParamHandle* mIntensityHandle;
    GHMaterialParamHandle* mAmbientParamHandle;
};
