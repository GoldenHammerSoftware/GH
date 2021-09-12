// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallback.h"
#include "GHMDesc.h"

class GHMaterialParamHandle;
class GHRenderTarget;

class GHRenderTargetMaterialCallback : public GHMaterialCallback
{
public:
    GHRenderTargetMaterialCallback(GHMaterialParamHandle* paramHandle,
                                   GHRenderTarget& renderTarget,
								   GHMDesc::WrapMode wrapMode = GHMDesc::WM_CLAMP);
    virtual ~GHRenderTargetMaterialCallback(void);
    
    virtual void apply(const void* data);
    
private:
    GHMaterialParamHandle* mParamHandle;
    GHRenderTarget& mRenderTarget;
	GHMDesc::WrapMode mWrapMode;
};
