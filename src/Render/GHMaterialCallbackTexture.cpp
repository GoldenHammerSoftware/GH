// Copyright Golden Hammer Software
#include "GHMaterialCallbackTexture.h"
#include "GHMaterialParamHandle.h"

GHMaterialCallbackTexture::GHMaterialCallbackTexture(GHMaterialParamHandle* paramHandle, GHTexture* tex)
	: mParamHandle(paramHandle)
	, mTexture(tex)
{
}

GHMaterialCallbackTexture::GHMaterialCallbackTexture(GHMaterialParamHandle* paramHandle, GHTexture** texRef)
	: mParamHandle(paramHandle)
	, mTextureRef(texRef)
{
}

GHMaterialCallbackTexture::~GHMaterialCallbackTexture(void)
{
	delete mParamHandle;
	// should we be managing the ref count on mTexture here?
}

void GHMaterialCallbackTexture::apply(const void* data)
{
	if (mTexture != nullptr)
	{
		GHMaterialParamHandle::TextureHandle texHandle(mTexture, GHMDesc::WM_WRAP);
		mParamHandle->setValue(GHMaterialParamHandle::HT_TEXTURE, &texHandle);
	}
	else if (mTextureRef != nullptr)
	{
		GHMaterialParamHandle::TextureHandle texHandle(*mTextureRef, GHMDesc::WM_WRAP);
		mParamHandle->setValue(GHMaterialParamHandle::HT_TEXTURE, &texHandle);
	}
}
