// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallbackFactory.h"
#include "GHString/GHString.h"
#include "GHMaterialCallbackType.h"

class GHTexture;

// pushes a texture as a shader arg
class GHMaterialCallbackFactoryTexture : public GHMaterialCallbackFactory
{
public:
	GHMaterialCallbackFactoryTexture(const char* paramName, GHMaterialCallbackType::Enum cbType, GHTexture* tex);
	GHMaterialCallbackFactoryTexture(const char* paramName, GHMaterialCallbackType::Enum cbType, GHTexture** texRef);
	virtual void createCallbacks(GHMaterial& mat) const;

private:
	// Raw pointer to a texture.  If this exists it is what we pass to the shader.
	GHTexture * mTexture{ nullptr };
	// Reference to an externally controlled texture.
	// If mTexture does not exist and mTextureRef exists instead, we pass the ref to the shader.
	// Allows for objects to replace the texture after this callback has been created.
	GHTexture** mTextureRef{ nullptr };

	GHString mParamName;
	GHMaterialCallbackType::Enum mCBType;
};
