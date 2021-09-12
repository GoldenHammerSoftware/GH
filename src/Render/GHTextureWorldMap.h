// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

class GHTexture;

// maps the pixels in a texture to world coordinates.
class GHTextureWorldMap
{
public:
	GHTextureWorldMap(GHTexture* tex, const GHPoint2& worldSize, const GHPoint2& worldOrigin);
	~GHTextureWorldMap(void);

	void getPixel(const GHPoint2& worldPos, GHPoint3& val);

private:
	GHTexture* mTexture;
	GHPoint2 mWorldSize;
	GHPoint2 mWorldOrigin;

	void* mBuffer;
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mDepth;
};
