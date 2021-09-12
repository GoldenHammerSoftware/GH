// Copyright Golden Hammer Software
#pragma once

#include <stddef.h> // size_t

struct GHTextureData;

// Interface for something that can create a texture data.
// Subclasses are intended to read the appropriate data format and extract the buffers and info.
class GHTextureDataFactory
{
public:
	virtual ~GHTextureDataFactory(void) = default;

	virtual GHTextureData* createFromMemory(void* data, size_t dataSize) const = 0;
};


