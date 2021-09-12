// Copyright Golden Hammer Software
#include "GHMaterialCallbackFactoryTime.h"
#include "GHMaterialCallbackTime.h"
#include "GHMaterial.h"
#include "GHMaterialCallbackType.h"

GHMaterialCallbackFactoryTime::GHMaterialCallbackFactoryTime(const GHTimeVal& time,
                                                             const char* paramName)
: mTime(time)
{
    mParamName.setConstChars(paramName, GHString::CHT_COPY);
}

void GHMaterialCallbackFactoryTime::createCallbacks(GHMaterial& mat) const
{
    GHMaterialParamHandle* paramHandle = mat.getParamHandle(mParamName.getChars());
    if (!paramHandle) return;
    
    GHMaterialCallbackTime* callback = new GHMaterialCallbackTime(paramHandle, mTime);
    
    mat.addCallback(GHMaterialCallbackType::CT_PERFRAME, callback);
}
