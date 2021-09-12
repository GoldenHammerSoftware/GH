// Copyright Golden Hammer Software
#include "GHMaterialCallbackTransform.h"
#include "GHMaterial.h"
#include "GHViewInfo.h"
#include "GHMaterialParamHandle.h"

GHMaterialCallbackTransform::GHMaterialCallbackTransform(GHMaterialParamHandle* modelViewProjHandle,
                                                         GHMaterialParamHandle* modelToWorldHandle,
                                                         GHMaterialParamHandle* worldToModelHandle,
                                                         GHMaterialParamHandle* eyePosModelHandle,
														 GHMaterialParamHandle* eyeDirModelHandle)
: mModelViewProjHandle(modelViewProjHandle)
, mModelToWorldHandle(modelToWorldHandle)
, mWorldToModelHandle(worldToModelHandle)
, mEyePosModelHandle(eyePosModelHandle)
, mEyeDirModelHandle(eyeDirModelHandle)
{
}

GHMaterialCallbackTransform::~GHMaterialCallbackTransform(void)
{
    delete mEyePosModelHandle;
	delete mEyeDirModelHandle;
    delete mWorldToModelHandle;
    delete mModelToWorldHandle;
    delete mModelViewProjHandle;
}

void GHMaterialCallbackTransform::apply(const void* data)
{
    GHMaterialCallback::CBVal_PerTrans* cbArg = (GHMaterialCallback::CBVal_PerTrans*)data;
    
	if (mModelViewProjHandle)
	{
		GHTransform modelViewProj;
		cbArg->mModelToWorld.mult(cbArg->mViewInfo.getDeviceTransforms().mViewProjTransform, modelViewProj);
        mModelViewProjHandle->setValue(GHMaterialParamHandle::HT_MAT16, modelViewProj.getMatrix().getCoords());
	}
	if (mModelToWorldHandle)
	{
        mModelToWorldHandle->setValue(GHMaterialParamHandle::HT_MAT16, cbArg->mModelToWorld.getMatrix().getCoords());
	}
    if (mWorldToModelHandle || mEyePosModelHandle)
    {
        GHTransform worldToModelTrans(cbArg->mModelToWorld);
        worldToModelTrans.invert();
        if (mWorldToModelHandle)
        {
            mWorldToModelHandle->setValue(GHMaterialParamHandle::HT_MAT16,
                                          worldToModelTrans.getMatrix().getCoords());
        }
        if (mEyePosModelHandle)
        {
            GHPoint3 eyePos;
			cbArg->mViewInfo.getDeviceTransforms().mViewInvTransform.getTranslate(eyePos);
            worldToModelTrans.mult(eyePos, eyePos);
            mEyePosModelHandle->setValue(GHMaterialParamHandle::HT_VEC3,
                                         eyePos.getCoords());
            
        }
		if (mEyeDirModelHandle)
		{
			GHPoint3 dir(0,0,1);
			cbArg->mViewInfo.getDeviceTransforms().mViewInvTransform.multDir(dir, dir);
			mEyeDirModelHandle->setValue(GHMaterialParamHandle::HT_VEC3,
										 dir.getCoords());
		}
    }
}
