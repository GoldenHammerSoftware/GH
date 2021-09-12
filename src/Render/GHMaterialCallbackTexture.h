// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallback.h"

class GHMaterialParamHandle;
class GHTexture;

// Passes a texture to a shader.
class GHMaterialCallbackTexture : public GHMaterialCallback
{
public:
	GHMaterialCallbackTexture(GHMaterialParamHandle* paramHandle, GHTexture* tex);
	// alternate constructor that takes a pointer to a texture.
	// Allows for objects to replace the texture after this callback has been created.
	GHMaterialCallbackTexture(GHMaterialParamHandle* paramHandle, GHTexture** texRef);
	virtual ~GHMaterialCallbackTexture(void);

	virtual void apply(const void* data);

private:
	GHMaterialParamHandle* mParamHandle{ nullptr };
	// Raw pointer to a texture.  If this exists it is what we pass to the shader.
	GHTexture* mTexture{ nullptr };
	// Reference to an externally controlled texture.
	// If mTexture does not exist and mTextureRef exists instead, we pass the ref to the shader.
	// Allows for objects to replace the texture after this callback has been created.
	GHTexture** mTextureRef{ nullptr };
};

