// Copyright Golden Hammer Software
#include "GHBillboardTransformAdjuster.h"
#include "GHViewInfo.h"

const GHTransform& GHBillboardTransformAdjuster::adjustTransform(const GHTransform& transform, 
                                                                 GHMDesc::BillboardType billboardType,
                                                                 const GHViewInfo& viewInfo,
                                                                 GHTransform& scratchTransform) const
{
    if (billboardType == GHMDesc::BT_NONE) return transform;
    if (billboardType == GHMDesc::BT_FULL)
    {
        scratchTransform = viewInfo.getEngineTransforms().mBillboardTransform;
    }
    else
    {
		scratchTransform = viewInfo.getEngineTransforms().mBillboardUpTransform;
    }
    GHPoint3 transPos;
    transform.getTranslate(transPos);
    scratchTransform.setTranslate(transPos);
    return scratchTransform;
}

