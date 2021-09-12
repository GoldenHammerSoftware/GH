#pragma once

#include "GHMaterialCallback.h"
#include "GHMath/GHPoint.h"

class GHMaterialParamHandle;

class GHMaterialCallbackShadowInfo : public GHMaterialCallback
{
public:
	GHMaterialCallbackShadowInfo(const GHPoint3& center, float radius, 
		GHMaterialParamHandle* centerHandle, GHMaterialParamHandle* radiusHandle);

	virtual void apply(const void* data);

private:
	const GHPoint3& mCenter;
	float mRadius;
	GHMaterialParamHandle* mCenterHandle{ 0 };
	GHMaterialParamHandle* mRadiusHandle{ 0 };
};
