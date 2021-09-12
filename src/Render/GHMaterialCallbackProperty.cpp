// Copyright Golden Hammer Software
#include "GHMaterialCallbackProperty.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHMath/GHPoint.h"

GHMaterialCallbackProperty::GHMaterialCallbackProperty(GHMaterialParamHandle* paramHandle,
                                                       GHMaterialParamHandle::HandleType type,
                                                       GHIdentifier propId,
                                                       const GHPropertyContainer* propOverride)
: mParamHandle(paramHandle)
, mPropId(propId)
, mHandleType(type)
, mPropOverride(propOverride)
{
}

GHMaterialCallbackProperty::~GHMaterialCallbackProperty(void)
{
    delete mParamHandle;
}

void GHMaterialCallbackProperty::apply(const void* data)
{
	GHMaterialCallback::CBVal_PerObj* cbVal = (GHMaterialCallback::CBVal_PerObj*)data;
	const GHPropertyContainer* props = 0;
	const void* targProp = 0;
	float propValF = 0;
	int propValI = 0;

	if (cbVal) {
		props = cbVal->mProps;
		if (props) {
			const GHProperty& targetProperty = props->getProperty(mPropId);
			if (targetProperty.isValid())
			{
				targProp = targetProperty;
				propValF = targetProperty;
				propValI = targetProperty;
			}
		}
	}
	if (mPropOverride)
	{
		props = mPropOverride;
		if (props) {
			const GHProperty& targetProperty = props->getProperty(mPropId);
			if (targetProperty.isValid())
			{
				targProp = targetProperty;
				propValF = targetProperty;
				propValI = targetProperty;
			}
		}
	}
	if (!props)
	{
		return;
	}

    if (!targProp && mHandleType != GHMaterialParamHandle::HT_FLOAT) return;
    
	if (mHandleType == GHMaterialParamHandle::HT_TEXTURE)
	{
		GHMaterialParamHandle::TextureHandle texHandle((GHTexture*)targProp, GHMDesc::WM_WRAP);
		mParamHandle->setValue(mHandleType, &texHandle);
	}
    else if (mHandleType == GHMaterialParamHandle::HT_MAT16)
    {
        const GHPoint<float, 16>* pointProp = (GHPoint<float, 16>*)targProp;
        mParamHandle->setValue(mHandleType, pointProp->getCoords());
    }
    else if (mHandleType == GHMaterialParamHandle::HT_VEC4)
    {
        const GHPoint<float, 4>* pointProp = (GHPoint<float, 4>*)targProp;
        mParamHandle->setValue(mHandleType, pointProp->getCoords());
    }
    else if (mHandleType == GHMaterialParamHandle::HT_VEC3)
    {
        const GHPoint<float, 3>* pointProp = (GHPoint<float, 3>*)targProp;
        mParamHandle->setValue(mHandleType, pointProp->getCoords());
    }
    else if (mHandleType == GHMaterialParamHandle::HT_VEC2)
    {
        const GHPoint<float, 2>* pointProp = (GHPoint<float, 2>*)targProp;
        mParamHandle->setValue(mHandleType, pointProp->getCoords());
    }
    else if (mHandleType == GHMaterialParamHandle::HT_FLOAT)
    {
        mParamHandle->setValue(mHandleType, &propValF);
    }
	else if (mHandleType == GHMaterialParamHandle::HT_INT)
	{
		mParamHandle->setValue(mHandleType, &propValI);
	}
    else 
    {
        mParamHandle->setValue(mHandleType, targProp);
    }
}
