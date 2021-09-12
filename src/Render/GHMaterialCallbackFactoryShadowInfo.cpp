#include "GHMaterialCallbackFactoryShadowInfo.h"
#include "GHMaterialCallbackShadowInfo.h"
#include "GHMaterial.h"

GHMaterialCallbackFactoryShadowInfo::GHMaterialCallbackFactoryShadowInfo(const char* prefix, const GHPoint3& center, float radius)
	: mPrefix(prefix)
	, mCenter(center)
	, mRadius(radius)
{
	::snprintf(mCenterName, sMaxPropNameLength, "%s%s", prefix, "Center");
	::snprintf(mRadiusName, sMaxPropNameLength, "%s%s", prefix, "Radius");
}

void GHMaterialCallbackFactoryShadowInfo::createCallbacks(GHMaterial& mat) const
{
	GHMaterialParamHandle* centerHandle = mat.getParamHandle(mCenterName);
	GHMaterialParamHandle* radiusHandle = mat.getParamHandle(mRadiusName);
	if (centerHandle || radiusHandle)
	{
		GHMaterialCallbackShadowInfo* cb = new GHMaterialCallbackShadowInfo(mCenter, mRadius, centerHandle, radiusHandle);
		mat.addCallback(GHMaterialCallbackType::CT_PERFRAME, cb);
	}
}

