// Copyright Golden Hammer Software
#include "GHMaterialCallbackFactoryViewInfo.h"
#include <stdio.h>
#include "GHMaterialParamHandle.h"
#include "GHMaterial.h"
#include "GHMaterialCallbackViewInfo.h"
#include "GHMaterialCallbackType.h"

GHMaterialCallbackFactoryViewInfo::GHMaterialCallbackFactoryViewInfo(const GHViewInfo& viewInfo,
                                                                     const char* prefixStr)
: mViewInfo(viewInfo)
{
    ::snprintf(mProjName, sMaxPropNameLength, "%s%s", prefixStr, "Proj");
	::snprintf(mProjInvName, sMaxPropNameLength, "%s%s", prefixStr, "ProjInv");
    ::snprintf(mViewName, sMaxPropNameLength, "%s%s", prefixStr, "View");
    ::snprintf(mViewProjName, sMaxPropNameLength, "%s%s", prefixStr, "ViewProj");
    ::snprintf(mViewInvName, sMaxPropNameLength, "%s%s", prefixStr, "ViewInv");
    ::snprintf(mEyeDirName, sMaxPropNameLength, "%s%s", prefixStr, "EyeDir");
    ::snprintf(mEyePosName, sMaxPropNameLength, "%s%s", prefixStr, "EyePos");
}

void GHMaterialCallbackFactoryViewInfo::createCallbacks(GHMaterial& mat) const
{
    GHMaterialParamHandle* projHandle = mat.getParamHandle(mProjName);
	GHMaterialParamHandle* projInvHandle = mat.getParamHandle(mProjInvName);
    GHMaterialParamHandle* viewHandle = mat.getParamHandle(mViewName);
    GHMaterialParamHandle* viewProjHandle = mat.getParamHandle(mViewProjName);
    GHMaterialParamHandle* viewInvHandle = mat.getParamHandle(mViewInvName);
    GHMaterialParamHandle* eyeDirHandle = mat.getParamHandle(mEyeDirName);
    GHMaterialParamHandle* eyePosHandle = mat.getParamHandle(mEyePosName);
    
    if (projHandle || viewHandle || viewProjHandle || viewInvHandle || eyeDirHandle || eyePosHandle)
    {
        GHMaterialCallbackViewInfo* cb = new GHMaterialCallbackViewInfo(mViewInfo,
                                                                        projHandle,
																		projInvHandle,
                                                                        viewHandle,
                                                                        viewProjHandle,
                                                                        viewInvHandle,
                                                                        eyeDirHandle, 
                                                                        eyePosHandle);
        mat.addCallback(GHMaterialCallbackType::CT_PERFRAME, cb);
    }
}
