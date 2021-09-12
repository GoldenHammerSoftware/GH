// Copyright Golden Hammer Software
#include "GHMaterialCallbackViewInfo.h"
#include "GHMaterialParamHandle.h"
#include "GHViewInfo.h"

GHMaterialCallbackViewInfo::GHMaterialCallbackViewInfo(const GHViewInfo& viewInfo,
                                                       GHMaterialParamHandle* projHandle,
													   GHMaterialParamHandle* projInvHandle,
                                                       GHMaterialParamHandle* viewHandle,
                                                       GHMaterialParamHandle* viewProjHandle,
                                                       GHMaterialParamHandle* viewInvHandle,
                                                       GHMaterialParamHandle* eyeDirHandle,
                                                       GHMaterialParamHandle* eyePosHandle)
: mViewInfo(viewInfo)
, mProjHandle(projHandle)
, mProjInvHandle(projInvHandle)
, mViewHandle(viewHandle)
, mViewProjHandle(viewProjHandle)
, mViewInvHandle(viewInvHandle)
, mEyeDirHandle(eyeDirHandle)
, mEyePosHandle(eyePosHandle)
{
}

GHMaterialCallbackViewInfo::~GHMaterialCallbackViewInfo(void)
{
    delete mEyePosHandle;
    delete mEyeDirHandle;
    delete mViewInvHandle;
    delete mViewProjHandle;
    delete mViewHandle;
    delete mProjHandle;
	delete mProjInvHandle;
}

void GHMaterialCallbackViewInfo::apply(const void* data)
{
    if (mProjHandle) {
        mProjHandle->setValue(GHMaterialParamHandle::HT_MAT16, 
			mViewInfo.getDeviceTransforms().mProjectionTransform.getMatrix().getCoords());
    }
	if (mProjInvHandle) {
		mProjInvHandle->setValue(GHMaterialParamHandle::HT_MAT16,
			mViewInfo.getDeviceTransforms().mProjectionInvTransform.getMatrix().getCoords());
	}
	if (mViewHandle) {
        mViewHandle->setValue(GHMaterialParamHandle::HT_MAT16, 
			mViewInfo.getDeviceTransforms().mViewTransform.getMatrix().getCoords());
    }
    if (mViewProjHandle) {
        mViewProjHandle->setValue(GHMaterialParamHandle::HT_MAT16, 
			mViewInfo.getDeviceTransforms().mViewProjTransform.getMatrix().getCoords());
    }
    if (mViewInvHandle) {
        mViewInvHandle->setValue(GHMaterialParamHandle::HT_MAT16, 
			mViewInfo.getDeviceTransforms().mViewInvTransform.getMatrix().getCoords());
    }
    if (mEyeDirHandle) {
        GHPoint3 eyeDir(0,0,-1);
		mViewInfo.getDeviceTransforms().mViewInvTransform.multDir(eyeDir, eyeDir);
        mEyeDirHandle->setValue(GHMaterialParamHandle::HT_VEC3,
                                eyeDir.getCoords());
    }
    if (mEyePosHandle) {
        GHPoint3 eyePos;
		mViewInfo.getDeviceTransforms().mViewInvTransform.getTranslate(eyePos);
        mEyePosHandle->setValue(GHMaterialParamHandle::HT_VEC3,
                                eyePos.getCoords());
    }
}
