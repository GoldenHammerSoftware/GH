// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallback.h"
#include "GHString/GHIdentifier.h"
#include "GHMaterialParamHandle.h"

class GHPropertyContainer;

class GHMaterialCallbackProperty : public GHMaterialCallback
{
public:
    GHMaterialCallbackProperty(GHMaterialParamHandle* paramHandle,
                               GHMaterialParamHandle::HandleType type,
                               GHIdentifier propId,
                               const GHPropertyContainer* propOverride);
    virtual ~GHMaterialCallbackProperty(void);
    
    virtual void apply(const void* data);
    
private:
    GHMaterialParamHandle* mParamHandle;
    GHIdentifier mPropId;
    GHMaterialParamHandle::HandleType mHandleType;
    const GHPropertyContainer* mPropOverride;
};
