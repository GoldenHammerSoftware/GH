// Copyright Golden Hammer Software
#pragma once

#include <cassert>
#include "GHMDesc.h"

// a way to push a value to a material.
class GHMaterialParamHandle
{
public:
    enum HandleType
    {
        HT_TEXTURE,
        HT_MAT16,
		HT_MAT16x64,
        HT_VEC4,
        HT_VEC3,
        HT_VEC2,
        HT_FLOAT,
		HT_INT,
	};
    
	struct TextureHandle
	{
		TextureHandle(GHTexture* tex, GHMDesc::WrapMode wrapMode) 
			: mTexture(tex)
			, mWrapMode(wrapMode)
		{}

		GHTexture* mTexture;
		GHMDesc::WrapMode mWrapMode;
	};

public:
    virtual ~GHMaterialParamHandle(void) {}
    virtual void setValue(HandleType type, const void* value) = 0;

	static unsigned int calcHandleSizeBytes(GHMaterialParamHandle::HandleType ht)
	{
		if (ht == GHMaterialParamHandle::HT_TEXTURE) return 0;
		if (ht == GHMaterialParamHandle::HT_MAT16) return 16*sizeof(float);
		if (ht == GHMaterialParamHandle::HT_MAT16x64) return 16*sizeof(float)*64;
		if (ht == GHMaterialParamHandle::HT_VEC4) return 4*sizeof(float);
		if (ht == GHMaterialParamHandle::HT_VEC3) return 3*sizeof(float);
		if (ht == GHMaterialParamHandle::HT_VEC2) return 2*sizeof(float);
		if (ht == GHMaterialParamHandle::HT_FLOAT) return sizeof(float);
		if (ht == GHMaterialParamHandle::HT_INT) return sizeof(int);
		assert(false);
		return 0;
	}

	//the sizeInBytes of a single element must be passed in, not the total size of elements*count
	static GHMaterialParamHandle::HandleType calcHandleType(unsigned int sizeInBytes)
	{
		if (sizeInBytes == 16 * sizeof(float)) return GHMaterialParamHandle::HT_MAT16;
		else if (sizeInBytes == 16 * sizeof(float) * 64) return GHMaterialParamHandle::HT_MAT16x64;
		else if (sizeInBytes == 4 * sizeof(float)) return GHMaterialParamHandle::HT_VEC4;
		else if (sizeInBytes == 3 * sizeof(float)) return GHMaterialParamHandle::HT_VEC3;
		else if (sizeInBytes == 2 * sizeof(float)) return GHMaterialParamHandle::HT_VEC2;
		else if (sizeInBytes == sizeof(float)) return GHMaterialParamHandle::HT_FLOAT;
		assert(false);
		return HT_FLOAT;
	}

	static GHMaterialParamHandle::HandleType calcIntHandleType(unsigned int sizeInBytes)
	{
		assert(sizeInBytes == sizeof(int) && "Error in calcIntHandleType: only supporting single int types (you may add more if needed)");
		return GHMaterialParamHandle::HT_INT;
	}
};
