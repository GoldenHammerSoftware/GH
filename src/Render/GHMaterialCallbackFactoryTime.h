// Copyright Golden Hammer Software
#pragma once

#include "GHMaterialCallbackFactory.h"
#include "GHString/GHString.h"

class GHTimeVal;

class GHMaterialCallbackFactoryTime : public GHMaterialCallbackFactory
{
public:
    GHMaterialCallbackFactoryTime(const GHTimeVal& time, const char* paramName);
    virtual void createCallbacks(GHMaterial& mat) const;
    
private:
    const GHTimeVal& mTime;
    GHString mParamName;
};
