#pragma once

#include "GHBulletContactAddedFrictionOverride.h"
#include "GHMath/GHRect.h"
#include <vector>
#include "GHMath/GHTransform.h"

class GHTexture;
class GHBulletPhysicsObject;

class GHBulletTextureLookupFrictionOverride : public GHBulletContactAddedFrictionOverride
{
public:
	struct ColorToFrictionPair
	{
		unsigned char mColor;
		float mFriction;
		ColorToFrictionPair(unsigned char color, float friction) : mColor(color), mFriction(friction) { }
		ColorToFrictionPair(void) : mColor(0), mFriction(0) { }
	};
	typedef std::vector<ColorToFrictionPair> ColorToFrictionMap;

	//for now, always looking at XZ plane for texture lookup. Possibly may want to add another parameter to handle other mapping
	GHBulletTextureLookupFrictionOverride(GHTexture* texture, 
										  const GHRect<float, 2>& bounds, const ColorToFrictionMap& colorToFrictionMap);
	~GHBulletTextureLookupFrictionOverride(void);

	virtual float getFriction(btManifoldPoint& cp, bool weAreFirstObj);
	virtual GHBulletContactAddedFrictionOverride* clone(void);

private:
	void calculateUVs(const GHPoint2& colPos, GHPoint2& outUVs);
	unsigned char lookupColorVal(const GHPoint2& uvs);
	float mapColorToFriction(unsigned char color);
	float binarySearchColorToFrictionMap(unsigned char color, int lowerBoundIndex, int upperBoundIndex);
	void sortColorToFrictionMap(void);

private:
	GHTexture* mTexture;
	GHRect<float, 2> mBounds;
	GHPoint2 mDivByBoundsSize;
	ColorToFrictionMap mColorToFrictionMap;
};
