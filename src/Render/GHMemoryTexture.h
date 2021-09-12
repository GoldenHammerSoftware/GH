// Copyright Golden Hammer Software
#pragma once

#include "GHTexture.h"

// A texture that isn't meant to be rendered, unless it is subclassed.
class GHMemoryTexture : public GHTexture
{
public:
    GHMemoryTexture(unsigned int width, unsigned int height, unsigned int depth);
    GHMemoryTexture(unsigned char* pTexels, unsigned int width, unsigned int height, unsigned int depth);
	virtual	~GHMemoryTexture(void);

    virtual bool lockSurface(void** ret, unsigned int& pitch);
	virtual bool unlockSurface(void);
	virtual bool getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth);

    // doesn't really make sense for memory texture.
	virtual void deleteSourceData(void) {}
	virtual void bind(void) {}
    
private:
	unsigned char*	mpTexels;
	unsigned int	mWidth;
	unsigned int	mHeight;
	unsigned int	mDepth;
	bool			mLocked;
};
