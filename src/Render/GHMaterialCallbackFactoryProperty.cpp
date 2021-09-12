// Copyright Golden Hammer Software
#include "GHMaterialCallbackFactoryProperty.h"
#include "GHMaterialCallbackProperty.h"

GHMaterialCallbackFactoryProperty::GHMaterialCallbackFactoryProperty(const char* paramName, 
                                                                     GHMaterialCallbackType::Enum cbType,
                                                                     GHIdentifier prop, 
                                                                     GHMaterialParamHandle::HandleType handleType,
                                                                     const GHPropertyContainer* propOverride)
: mParamName(paramName, GHString::CHT_COPY)
, mPropId(prop)
, mCBType(cbType)
, mHandleType(handleType)
, mPropOverride(propOverride)
{
}

void GHMaterialCallbackFactoryProperty::createCallbacks(GHMaterial& mat) const
{
    GHMaterialParamHandle* paramHandle = mat.getParamHandle(mParamName);
	if (!paramHandle) {
		return;
	}
    
    GHMaterialCallbackProperty* cb = new GHMaterialCallbackProperty(paramHandle, mHandleType, mPropId, mPropOverride);
    mat.addCallback(mCBType, cb);
}
