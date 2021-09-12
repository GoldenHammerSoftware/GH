// Copyright Golden Hammer Software
#pragma once

#include "GHTextureDataFactory.h"

// Decoder for ktx files.
class GHTextureDataFactoryKTX : public GHTextureDataFactory
{
public:
	virtual GHTextureData* createFromMemory(void* data, size_t dataSize) const;
};
