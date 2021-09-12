// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResource.h"

class GHTexture : public GHResource
{
public:
	virtual void bind(void) = 0;

    // gain access to the texture pixels.
	// Todo: add support for mip level interaction.
    virtual bool lockSurface(void** ret, unsigned int& pitch) = 0;
	virtual bool unlockSurface(void) = 0;
	virtual bool getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth) = 0;
    
	// delete any data not required for rendering
	virtual void deleteSourceData(void) = 0;
    
    // We expect lockedSurface to come from lockSurface on the same texture.
    void* getPixel(unsigned int col, unsigned int row, void* lockedSurface);
};
