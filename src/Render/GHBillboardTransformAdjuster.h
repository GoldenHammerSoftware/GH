// Copyright Golden Hammer Software
#pragma once

#include "GHMDesc.h"

class GHTransform;
class GHViewInfo;

// widget class to decide what the transform for an object should be,
//  based on the billboard property, view info, and input transform
class GHBillboardTransformAdjuster
{
public:
    const GHTransform& adjustTransform(const GHTransform& transform, 
                                       GHMDesc::BillboardType billboardType,
                                       const GHViewInfo& viewInfo,
                                       GHTransform& scratchTransform) const;
};

