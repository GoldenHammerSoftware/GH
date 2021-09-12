// Copyright Golden Hammer Software
#include "GHRenderTargetMaterialCallbackFactory.h"
#include "GHRenderTargetMaterialCallback.h"

GHRenderTargetMaterialCallbackFactory::GHRenderTargetMaterialCallbackFactory(const char* paramName, 
	GHMaterialCallbackType::Enum cbType, GHRenderTarget& target, GHMDesc::WrapMode wrapMode)
: mCBType(cbType)
, mTarget(target)
, mParamName(paramName, GHString::CHT_COPY)
, mWrapMode(wrapMode)
{
}

void GHRenderTargetMaterialCallbackFactory::createCallbacks(GHMaterial& mat) const
{
    GHMaterialParamHandle* paramHandle = mat.getParamHandle(mParamName);
    if (!paramHandle) return;
    
    GHMaterialCallback* cb = new GHRenderTargetMaterialCallback(paramHandle, mTarget, mWrapMode);
    mat.addCallback(mCBType, cb);
}
