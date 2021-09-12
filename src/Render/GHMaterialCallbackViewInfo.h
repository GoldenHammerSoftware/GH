// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallback.h"

class GHViewInfo;
class GHMaterialParamHandle;

// A callback to set the view info's projection transforms.
class GHMaterialCallbackViewInfo : public GHMaterialCallback
{
public:
    GHMaterialCallbackViewInfo(const GHViewInfo& viewInfo,
                               GHMaterialParamHandle* projHandle,
							   GHMaterialParamHandle* projInvHandle,
                               GHMaterialParamHandle* viewHandle,
                               GHMaterialParamHandle* viewProjHandle,
                               GHMaterialParamHandle* viewInvHandle,
                               GHMaterialParamHandle* eyeDirHandle,
                               GHMaterialParamHandle* eyePosHandle);
    virtual ~GHMaterialCallbackViewInfo(void);

    virtual void apply(const void* data);
    
private:
    const GHViewInfo& mViewInfo;
    GHMaterialParamHandle* mProjHandle;
	GHMaterialParamHandle* mProjInvHandle;
    GHMaterialParamHandle* mViewHandle;
    GHMaterialParamHandle* mViewProjHandle;
    GHMaterialParamHandle* mViewInvHandle;
    GHMaterialParamHandle* mEyeDirHandle;
    GHMaterialParamHandle* mEyePosHandle;
};
