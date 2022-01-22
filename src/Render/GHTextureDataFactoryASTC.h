// Copyright Golden Hammer Software
#pragma once

#include "GHTextureDataFactory.h"
#include "GHTextureFormat.h"

// Decoder for astc files.
class GHTextureDataFactoryASTC : public GHTextureDataFactory
{
public:
	virtual GHTextureData* createFromMemory(void* data, size_t dataSize) const;
	
	GHTextureData* createFromMemoryWithMips(void* data, size_t dataSize, GHTextureFormat::Enum textureFormat, unsigned int width, unsigned int height, unsigned int mipCount, bool headerExists);
};
