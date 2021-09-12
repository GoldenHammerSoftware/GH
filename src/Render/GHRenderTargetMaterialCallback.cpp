// Copyright Golden Hammer Software
#include "GHRenderTargetMaterialCallback.h"
#include "GHMaterialParamHandle.h"
#include "GHRenderTarget.h"

GHRenderTargetMaterialCallback::GHRenderTargetMaterialCallback(GHMaterialParamHandle* paramHandle,
                                                               GHRenderTarget& renderTarget,
															   GHMDesc::WrapMode wrapMode)
: mParamHandle(paramHandle)
, mRenderTarget(renderTarget)
, mWrapMode(wrapMode)
{
}

GHRenderTargetMaterialCallback::~GHRenderTargetMaterialCallback(void)
{
    delete mParamHandle;
}

void GHRenderTargetMaterialCallback::apply(const void* data)
{
	GHMaterialParamHandle::TextureHandle texHandle(mRenderTarget.getTexture(), mWrapMode);
    mParamHandle->setValue(GHMaterialParamHandle::HT_TEXTURE, &texHandle);
}

