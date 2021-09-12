#pragma once

#include "GHMaterialCallbackFactory.h"
#include "GHMath/GHPoint.h"

class GHMaterialCallbackFactoryShadowInfo : public GHMaterialCallbackFactory
{
public:
	GHMaterialCallbackFactoryShadowInfo(const char* prefix, const GHPoint3& center, float radius);

	virtual void createCallbacks(GHMaterial& mat) const;

private:
	const char* mPrefix{ 0 };
	const GHPoint3& mCenter;
	float mRadius{ 0 };

	static const int sMaxPropNameLength = 128;
	char mCenterName[sMaxPropNameLength];
	char mRadiusName[sMaxPropNameLength];
};
