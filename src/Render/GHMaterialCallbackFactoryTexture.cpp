#include "GHMaterialCallbackFactoryTexture.h"
#include "GHMaterialCallbackTexture.h"
#include "GHMaterialParamHandle.h"
#include "GHMaterial.h"

GHMaterialCallbackFactoryTexture::GHMaterialCallbackFactoryTexture(const char* paramName, GHMaterialCallbackType::Enum cbType, GHTexture* tex)
	: mParamName(paramName, GHString::CHT_COPY)
	, mTexture(tex)
	, mCBType(cbType)
{
}

GHMaterialCallbackFactoryTexture::GHMaterialCallbackFactoryTexture(const char* paramName, GHMaterialCallbackType::Enum cbType, GHTexture** texRef)
	: mParamName(paramName, GHString::CHT_COPY)
	, mTextureRef(texRef)
	, mCBType(cbType)
{
}

void GHMaterialCallbackFactoryTexture::createCallbacks(GHMaterial& mat) const
{
	GHMaterialParamHandle* paramHandle = mat.getParamHandle(mParamName);
	if (!paramHandle) return;

	GHMaterialCallbackTexture* cb = nullptr;
	if (mTexture)
	{
		cb = new GHMaterialCallbackTexture(paramHandle, mTexture);
	}
	else if (mTextureRef)
	{
		cb = new GHMaterialCallbackTexture(paramHandle, mTextureRef);
	}
	mat.addCallback(mCBType, cb);
}
