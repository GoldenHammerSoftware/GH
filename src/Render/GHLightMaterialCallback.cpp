// Copyright Golden Hammer Software
#include "GHLightMaterialCallback.h"
#include "GHMath/GHPoint.h"
#include "GHLightMgr.h"
#include "GHMath/GHTransform.h"
#include "GHMaterialParamHandle.h"
#include "GHMaterial.h"
#include "GHPlatform/GHDebugMessage.h"

GHLightMaterialCallback::GHLightMaterialCallback(const GHLightMgr& lightMgr, int lightIndex,
	GHMaterialParamHandle* dirHandle,
	GHMaterialParamHandle* intensityHandle,
	GHMaterialParamHandle* ambientParamHandle)
: mLightMgr(lightMgr)
, mLightIndex(lightIndex)
, mDirHandle(dirHandle)
, mIntensityHandle(intensityHandle)
, mAmbientParamHandle(ambientParamHandle)
{
}

GHLightMaterialCallback::~GHLightMaterialCallback(void)
{
    delete mDirHandle;
	delete mIntensityHandle;
	delete mAmbientParamHandle;
}

void GHLightMaterialCallback::apply(const void* data)
{
    GHMaterialCallback::CBVal_PerTrans* cbArg = (GHMaterialCallback::CBVal_PerTrans*)data;
    
	const GHLightMgr::GHLight& light = mLightMgr.getLight(mLightIndex);
	if (mDirHandle)
    {
		// directional light.
		// todo: point light
		const GHTransform& worldTrans = cbArg->mModelToWorld;
		GHTransform invWorld(worldTrans);
		invWorld.invert();
		GHPoint3 modelLight;
		invWorld.multDir(light.mVec, modelLight);
		modelLight.normalize();
		mDirHandle->setValue(GHMaterialParamHandle::HT_VEC3, modelLight.getCoords());
	}
	if (mIntensityHandle)
	{
		mIntensityHandle->setValue(GHMaterialParamHandle::HT_VEC3, light.mIntensity.getCoords());
	}
    if (mAmbientParamHandle)
    {
        float ambientAmount = mLightMgr.getAmbientAmount();
        mAmbientParamHandle->setValue(GHMaterialParamHandle::HT_FLOAT, (const void*)&ambientAmount);
    }
}
