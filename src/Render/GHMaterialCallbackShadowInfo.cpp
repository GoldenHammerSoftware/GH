#include "GHMaterialCallbackShadowInfo.h"
#include "GHMaterialParamHandle.h"

GHMaterialCallbackShadowInfo::GHMaterialCallbackShadowInfo(const GHPoint3& center, float radius,
	GHMaterialParamHandle* centerHandle, GHMaterialParamHandle* radiusHandle)
	: mCenter(center)
	, mRadius(radius)
	, mCenterHandle(centerHandle)
	, mRadiusHandle(radiusHandle)
{

}

void GHMaterialCallbackShadowInfo::apply(const void* data)
{
	if (mCenterHandle) 
	{
		mCenterHandle->setValue(GHMaterialParamHandle::HT_VEC3, mCenter.getCoords());
	}
	if (mRadiusHandle)
	{
		mRadiusHandle->setValue(GHMaterialParamHandle::HT_FLOAT, &mRadius);
	}
}
