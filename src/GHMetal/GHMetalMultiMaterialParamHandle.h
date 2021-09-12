#pragma once

#include "GHMaterialParamHandle.h"

class GHMetalMultiMaterialParamHandle : public GHMaterialParamHandle
{
public:
    ~GHMetalMultiMaterialParamHandle(void);
    
    virtual void setValue(HandleType type, const void* value) override;
    
    void addSubHandle(GHMaterialParamHandle* param);
    
private:
    std::vector<GHMaterialParamHandle*> mSubHandles;
};
