#include "GHMetalMultiMaterialParamHandle.h"

GHMetalMultiMaterialParamHandle::~GHMetalMultiMaterialParamHandle(void)
{
    for (auto i = mSubHandles.begin(); i != mSubHandles.end(); ++i)
    {
        delete (*i);
    }
}

void GHMetalMultiMaterialParamHandle::setValue(HandleType type, const void* value)
{
    for (auto i = mSubHandles.begin(); i != mSubHandles.end(); ++i)
    {
        (*i)->setValue(type, value);
    }
}

void GHMetalMultiMaterialParamHandle::addSubHandle(GHMaterialParamHandle* param)
{
    mSubHandles.push_back(param);
}
