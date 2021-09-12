// Copyright Golden Hammer Software
#include "GHMaterialCallbackTime.h"
#include "GHMaterialParamHandle.h"
#include "GHPlatform/GHTimeVal.h"

GHMaterialCallbackTime::GHMaterialCallbackTime(GHMaterialParamHandle* paramHandle, const GHTimeVal& time)
: mParamHandle(paramHandle)
, mTime(time)
{
    
}

GHMaterialCallbackTime::~GHMaterialCallbackTime(void)
{
    delete mParamHandle;
}

void GHMaterialCallbackTime::apply(const void* data)
{
    float time = mTime.getTime();
    mParamHandle->setValue(GHMaterialParamHandle::HT_FLOAT, (const void*)&time);
}
