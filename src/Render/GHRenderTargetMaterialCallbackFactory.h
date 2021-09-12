// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallbackFactory.h"
#include "GHMaterial.h"
#include "GHString/GHString.h"
#include "GHMDesc.h"

class GHRenderTarget;

class GHRenderTargetMaterialCallbackFactory : public GHMaterialCallbackFactory
{
public:
    GHRenderTargetMaterialCallbackFactory(const char* paramName, 
                                          GHMaterialCallbackType::Enum cbType,
                                          GHRenderTarget& target,
										  GHMDesc::WrapMode wrapMode = GHMDesc::WM_CLAMP);
    
    virtual void createCallbacks(GHMaterial& mat) const;
    
private:
    GHString mParamName;
    GHMaterialCallbackType::Enum mCBType;
    GHRenderTarget& mTarget;
	GHMDesc::WrapMode mWrapMode;
};
