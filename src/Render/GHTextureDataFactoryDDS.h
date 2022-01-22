// Copyright Golden Hammer Software
#pragma once

#include "GHTextureDataFactory.h"

class GHTextureDataFactoryDDS : public GHTextureDataFactory
{
public:
	virtual GHTextureData* createFromMemory(void* data, size_t dataSize) const override;
};