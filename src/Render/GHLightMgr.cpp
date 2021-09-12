// Copyright Golden Hammer Software
#include "GHLightMgr.h"
#include "GHMaterialCallbackMgr.h"
#include "GHMaterialParamHandle.h"
#include "GHMaterial.h"
#include "GHLightMaterialCallback.h"

GHLightMgr::GHLightMgr(GHMaterialCallbackMgr& matCallbackMgr)
: mMatCallbackMgr(matCallbackMgr)
, mCallbackFactory(*this)
, mAmbientAmount(0)
{
    mMatCallbackMgr.addFactory(&mCallbackFactory);
}

GHLightMgr::~GHLightMgr(void)
{
    mMatCallbackMgr.removeFactory(&mCallbackFactory);
}

const GHLightMgr::GHLight& GHLightMgr::getLight(int index) const
{
	assert(index < sMaxLights);
    return mLights[index];
}

GHLightMgr::GHLight& GHLightMgr::getLight(int index) 
{
	assert(index < sMaxLights);
	return mLights[index];
}

// deprecated.  use getLight instead.
void GHLightMgr::setLightDir(int index, const GHPoint3& dir)
{
    if (index >= sMaxLights) return;
    mLights[index].mVec = dir;

	// this is the old form of the lights where we only had direction.  set up good default other values.
	mLights[index].mIntensity = GHPoint3(1.0f, 1.0f, 1.0f);
}

GHLightMgr::CallbackFactory::CallbackFactory(const GHLightMgr& lightMgr)
: mLightMgr(lightMgr)
{
}

void GHLightMgr::CallbackFactory::createCallbacks(GHMaterial& mat) const
{
    GHMaterialParamHandle* lightAmbientParamHandle = mat.getParamHandle("LightAmbientAmount");

	char paramName[16];
	for (int i = 0; i < sMaxLights; ++i)
	{
		::sprintf(paramName, "LightDir%d", i);
		GHMaterialParamHandle* lightDirParamHandle = mat.getParamHandle(paramName);
		if (!lightDirParamHandle && !lightAmbientParamHandle) continue;

		::sprintf(paramName, "LightIntensity%d", i);
		GHMaterialParamHandle* lightLumParamHandle = mat.getParamHandle(paramName);
		if (!lightDirParamHandle && !lightAmbientParamHandle && !lightLumParamHandle) continue;

		GHLightMaterialCallback* cb = new GHLightMaterialCallback(mLightMgr, i, lightDirParamHandle, lightLumParamHandle, lightAmbientParamHandle);
		mat.addCallback(GHMaterialCallbackType::CT_PERTRANS, cb);

		// only send the ambient on the first light.
		lightAmbientParamHandle = 0;
	}
}
