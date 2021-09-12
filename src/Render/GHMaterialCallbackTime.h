// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallback.h"

class GHMaterialParamHandle;
class GHTimeVal;

class GHMaterialCallbackTime : public GHMaterialCallback
{
public:
    GHMaterialCallbackTime(GHMaterialParamHandle* paramHandle, const GHTimeVal& time);
    virtual ~GHMaterialCallbackTime(void);
    
    virtual void apply(const void* data);
    
private:
    GHMaterialParamHandle* mParamHandle;
    const GHTimeVal& mTime;
};
