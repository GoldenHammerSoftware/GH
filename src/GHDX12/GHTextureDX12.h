#pragma once

#include "Render/GHTexture.h"

class GHTextureDX12 : public GHTexture
{
public:
	virtual void bind(void) override {}

	virtual bool lockSurface(void** ret, unsigned int& pitch) override { return false; }
	virtual bool unlockSurface(void) override { return false; }
	virtual bool getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth) override { return false; }

	virtual void deleteSourceData(void) override {}
};
