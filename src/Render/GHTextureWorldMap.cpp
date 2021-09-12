// Copyright Golden Hammer Software
#include "GHTextureWorldMap.h"
#include "GHTexture.h"
#include <math.h>
#include "GHPlatform/GHDebugMessage.h"

GHTextureWorldMap::GHTextureWorldMap(GHTexture* tex, const GHPoint2& worldSize, const GHPoint2& worldOrigin)
: mTexture(0)
, mWorldSize(worldSize)
, mWorldOrigin(worldOrigin)
{
	GHRefCounted::changePointer((GHRefCounted*&)mTexture, tex);
	if (mTexture)
	{
		unsigned int pitch;
		mTexture->lockSurface(&mBuffer, pitch);
		mTexture->getDimensions(mWidth, mHeight, mDepth);
	}
}

GHTextureWorldMap::~GHTextureWorldMap(void)
{
	if (mTexture) {
		mTexture->unlockSurface();
	}
	GHRefCounted::changePointer((GHRefCounted*&)mTexture, 0);
}

void GHTextureWorldMap::getPixel(const GHPoint2& worldPos, GHPoint3& val)
{
	if (!mBuffer) {
		val[0] = 1.0f;
		val[1] = 1.0f;
		val[2] = 1.0f;
		return;
	}

	assert(mWorldSize[0] > 0 && mWorldSize[1] > 0);
	GHPoint2 texPos = worldPos;
	texPos -= mWorldOrigin;
	texPos /= mWorldSize;
	texPos.clamp(GHPoint2(0, 0), GHPoint2(1, 1));
	texPos[0] *= mWidth;
	texPos[1] *= mHeight;
	unsigned char* pixelStart = (unsigned char*)mBuffer + (unsigned int)floor(texPos[0])*mDepth + (unsigned int)floor(texPos[1])*mWidth*mDepth;

	val[0] = (float)(*pixelStart)/255.0f;
	pixelStart++;
	val[1] = (float)(*pixelStart) / 255.0f;
	pixelStart++;
	val[2] = (float)(*pixelStart) / 255.0f;
}

