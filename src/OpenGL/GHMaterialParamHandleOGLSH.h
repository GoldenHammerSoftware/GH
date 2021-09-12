// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMaterialParamHandle.h"

class GHMaterialOGLSH;

class GHMaterialParamHandleOGLSH : public GHMaterialParamHandle
{
public:
    GHMaterialParamHandleOGLSH(int shaderHandle, GHMaterialOGLSH& mat);
    
    virtual void setValue(HandleType type, const void* value);
    
private:
    int mShaderHandle;
    GHMaterialOGLSH& mMat;
};
