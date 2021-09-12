// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallbackFactory.h"
#include "GHMaterial.h"
#include "GHString/GHString.h"
#include "GHMaterialParamHandle.h"
#include "GHMaterialCallbackType.h"

class GHPropertyContainer;

// Factory for filtering prop vals to shaders
class GHMaterialCallbackFactoryProperty : public GHMaterialCallbackFactory
{
public:
    GHMaterialCallbackFactoryProperty(const char* paramName, 
                                      GHMaterialCallbackType::Enum cbType,
                                      GHIdentifier prop, 
                                      GHMaterialParamHandle::HandleType handleType,
                                      const GHPropertyContainer* propOverride=0);

    virtual void createCallbacks(GHMaterial& mat) const;
    
private:
    GHString mParamName;
    GHIdentifier mPropId;
    GHMaterialCallbackType::Enum mCBType;
    GHMaterialParamHandle::HandleType mHandleType;
    const GHPropertyContainer* mPropOverride;
};
