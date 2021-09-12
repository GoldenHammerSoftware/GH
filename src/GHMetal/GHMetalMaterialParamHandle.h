#pragma once

#include "GHMaterialParamHandle.h"

class GHMetalMaterialParamHandle : public GHMaterialParamHandle
{
public:
    GHMetalMaterialParamHandle(void* buffer, size_t offset);
    
    virtual void setValue(HandleType type, const void* value) override;
    
private:
    void* mBuffer{nil};
    size_t mOffset{0}; // doubles as texture id for textures.
};
