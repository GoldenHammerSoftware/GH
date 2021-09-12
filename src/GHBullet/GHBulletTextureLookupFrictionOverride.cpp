#include "GHBulletTextureLookupFrictionOverride.h"
#include "GHBulletPhysicsObject.h"
#include "GHTexture.h"
#include "GHMath/GHFloat.h"
#include "GHMath/GHTransform.h"
#include "GHBulletBtInclude.h"
#include <algorithm>
#include "GHMath/GHMath.h"

/* some scratch code for calculating a single gradient from a set of rgb color values
#include "GHPlatform/GHDebugMessage.h"

GHPoint3 pointFromRGB(unsigned char r, unsigned char g, unsigned char b)
{
	GHPoint3 ret(255 - r, 255 - g, 255- b);
	ret /= 255.f;
	return ret;
}

void printOilValues(void)
{
	GHPoint3 mostOil = pointFromRGB(15, 36, 62);
	GHPoint3 leastOil = pointFromRGB(197, 217, 242);

	float mostLen = mostOil.length();
	float leastLen = leastOil.length();

	GHPoint3 oilColors[9] =
	{
		pointFromRGB(15, 36, 62),
		pointFromRGB(66, 86, 113),
		pointFromRGB(92,113, 138),
		pointFromRGB(114, 135, 160),
		pointFromRGB(149, 169, 194),
		pointFromRGB(158, 178, 203),
		pointFromRGB(169, 189, 213),
		pointFromRGB(193, 213, 238),
		pointFromRGB(197, 217, 242)
	};

	float oilAmounts[9];
	for (int i = 0; i < 9; ++i)
	{
		oilAmounts[i] = oilColors[i].length();
	}

	unsigned char oilCharVal[9];

	for (int i = 8; i >= 0; --i)
	{
		oilCharVal[i] = unsigned char((oilAmounts[i] / oilAmounts[0])*255.f);
		GHDebugMessage::outputString("%d: %f -> %d\n", i + 1, oilAmounts[i], oilCharVal[i]);
	}
}
*/

GHBulletTextureLookupFrictionOverride::GHBulletTextureLookupFrictionOverride(GHTexture* texture,
	const GHRect<float, 2>& bounds, const ColorToFrictionMap& colorToFrictionMap)
	: mBounds(bounds)
	, mColorToFrictionMap(colorToFrictionMap)
	, mTexture(0)
{
	//printOilValues();

	GHResource::changePointer((GHRefCounted*&)mTexture, texture);

	mDivByBoundsSize = mBounds.mMax;
	mDivByBoundsSize -= mBounds.mMin;

	for (int i = 0; i < 2; ++i)
	{
		if (!GHFloat::isZero(mDivByBoundsSize[i]))
		{
			mDivByBoundsSize[i] = 1.f / mDivByBoundsSize[i];
		}
		else
		{
			assert(false && "Error: Bounds should not be zero-sized in any dimension");
		}
	}

	sortColorToFrictionMap();
}

GHBulletContactAddedFrictionOverride* GHBulletTextureLookupFrictionOverride::clone(void)
{
	return new GHBulletTextureLookupFrictionOverride(mTexture, mBounds, mColorToFrictionMap);
}

GHBulletTextureLookupFrictionOverride::~GHBulletTextureLookupFrictionOverride(void)
{
	GHResource::changePointer((GHRefCounted*&)mTexture, 0);
}

float GHBulletTextureLookupFrictionOverride::getFriction(btManifoldPoint& cp, bool weAreFirstObj)
{
	GHPoint3 colPos;
	if (weAreFirstObj)
	{
		colPos.setCoords(cp.m_localPointA.x(), cp.m_localPointA.y(), cp.m_localPointA.z());
	}
	else
	{
		colPos.setCoords(cp.m_localPointB.x(), cp.m_localPointB.y(), cp.m_localPointB.z());
	}
	
	//note: current assumptions (would need to add more configuration if needed)
	// - the surface we compare is the XZ plane
	// - the rect bounds is 0-1 UVs in the texture.
	// - we only care about the zero-indexed (red?) channel

	GHPoint2 colPos2D(colPos[0], colPos[2]);
	GHPoint2 uvs;
	calculateUVs(colPos2D, uvs);

	unsigned char color = lookupColorVal(uvs);

	return mapColorToFriction(color);
}

void GHBulletTextureLookupFrictionOverride::calculateUVs(const GHPoint2& colPos, GHPoint2& outUVs)
{
	outUVs = colPos;
	outUVs -= mBounds.mMin;
	outUVs *= mDivByBoundsSize;

	GHMath::clamp(0.f, 1.f, outUVs[0]);
	GHMath::clamp(0.f, 1.f, outUVs[1]);
}

unsigned char GHBulletTextureLookupFrictionOverride::lookupColorVal(const GHPoint2& uvs)
{
	unsigned int width, height, depth;
	if (!mTexture->getDimensions(width, height, depth))
	{
//		assert(false && "Error getting dimensions of texture");
		return 0;
	}

	void* rawTexData;
	unsigned int pitch;
	if (!mTexture->lockSurface(&rawTexData, pitch))
	{
		assert(false && "Error locking texture");
		return 0;
	}

	width = (unsigned char)((float)width * uvs[0]);
	height = (unsigned char)((float)height * uvs[1]);

	void* pixel = mTexture->getPixel(width, height, rawTexData);
	if (!pixel)
	{
		return 0;
	}

	//we're always looking at the zero-indexed channel
	unsigned char *pixelVal = (unsigned char*)pixel;
	unsigned char ret = *pixelVal;

	mTexture->unlockSurface();

	return ret;
}

float GHBulletTextureLookupFrictionOverride::mapColorToFriction(unsigned char color)
{
	//if there is no map, return the color value as a normalized float
	if (mColorToFrictionMap.size() == 0)
	{
		return ((float)color)/255.f;
	}
	return binarySearchColorToFrictionMap(color, 0, (int)mColorToFrictionMap.size() - 1);
}

float GHBulletTextureLookupFrictionOverride::binarySearchColorToFrictionMap(unsigned char color, int lowerBoundIndex, int upperBoundIndex)
{
	if (upperBoundIndex == lowerBoundIndex)
	{
		return mColorToFrictionMap[upperBoundIndex].mFriction;
	}

	unsigned char upperBoundColor = mColorToFrictionMap[upperBoundIndex].mColor;
	if (upperBoundColor <= color)
	{
		return mColorToFrictionMap[upperBoundIndex].mFriction;
	}

	unsigned char lowerBoundColor = mColorToFrictionMap[lowerBoundIndex].mColor;
	if (lowerBoundColor >= color)
	{
		return mColorToFrictionMap[lowerBoundIndex].mFriction;
	}

	int middleIndex = ((upperBoundIndex - lowerBoundIndex) / 2) + lowerBoundIndex;

	if (middleIndex != lowerBoundIndex && middleIndex != upperBoundIndex)
	{
		unsigned char middleColor = mColorToFrictionMap[middleIndex].mColor;

		if (middleColor > color)
		{
			return binarySearchColorToFrictionMap(color, lowerBoundIndex, middleIndex);
		}
		else
		{
			return binarySearchColorToFrictionMap(color, middleIndex, upperBoundIndex);
		}
	}

	else //if (lowerBoundColor < color && color < upperBoundColor)
	{
		float distance = float(upperBoundColor - lowerBoundColor);
		float pct =  float(color - lowerBoundColor) / distance;
		float ret = pct*mColorToFrictionMap[upperBoundIndex].mFriction + (1.f - pct)*mColorToFrictionMap[lowerBoundIndex].mFriction;
		return ret;
	}
}

void GHBulletTextureLookupFrictionOverride::sortColorToFrictionMap(void)
{
	struct
	{
		bool operator()(const ColorToFrictionPair& lhs, const ColorToFrictionPair& rhs)
		{
			return lhs.mColor < rhs.mColor;
		}
	}localPredicate;

	std::sort(mColorToFrictionMap.begin(), mColorToFrictionMap.end(), localPredicate);
}
