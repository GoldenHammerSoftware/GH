// Copyright Golden Hammer Software
#include "GHMD3LoaderAnimModifierGPU.h"
#include "GHGPUVertexAnimController.h"
#include "GHMD3Structs.h"
#include "GHGeometry.h"

GHMD3LoaderAnimModifierGPU::GHMD3LoaderAnimModifierGPU(GHIdentifier interpProp,
                                                       const GHTimeVal& time)
: mInterpProp(interpProp)
, mTime(time)
{
}

void GHMD3LoaderAnimModifierGPU::modifyRootGeom(GHGeometry& geom, const GHMD3SurfaceDescription& surfaceDesc) const
{
    if (surfaceDesc.numframes > 1) geom.setCloneType(GHGeometry::CT_CLONEVB);
    else geom.setCloneType(GHGeometry::CT_SAMEVB);
}

GHFrameAnimControllerVB* GHMD3LoaderAnimModifierGPU::createAnimController(void) const
{
    unsigned int lowStreamTarget = 1;
    unsigned int highStreamTarget = 2;
    unsigned int sourceStreamId = 0;
    
    return new GHGPUVertexAnimController(lowStreamTarget, highStreamTarget,
                                         sourceStreamId, mInterpProp,
                                         mTime);
}

GHVBDescription* GHMD3LoaderAnimModifierGPU::createVBDescription(const GHMD3SurfaceDescription& surfaceDesc) const
{
    GHVBDescription* ret = new GHVBDescription(surfaceDesc.numverts, surfaceDesc.numtriangles*3, GHVBBlitter::BT_INDEX);
    
    GHVBDescription::StreamDescription streamDesc;
    streamDesc.mIsShared = true;
    streamDesc.mUsage = GHVBUsage::STATIC;
	if (surfaceDesc.numframes == 1)
    {
        // if there are no anim frames, we want everything in the root buffer.
        streamDesc.mComps.push_back(GHVertexComponent::AP_POS);
        //comps.push_back(GHVertexComponent::AP_ENCODEDNORMAL);
        streamDesc.mComps.push_back(GHVertexComponent::AP_NORMAL);
	}
    // otherwise just uv.  pos and normal come from anim frames.
    streamDesc.mComps.push_back(GHVertexComponent::AP_UV1);
    ret->addStreamDescription(streamDesc);
    
	return ret;
}

GHVBDescription* GHMD3LoaderAnimModifierGPU::createAnimInfoVBDescription(const GHMD3SurfaceDescription& surfaceDesc) const
{
    GHVBDescription* ret = new GHVBDescription(surfaceDesc.numverts, 0, GHVBBlitter::BT_UNKNOWN);

    GHVBDescription::StreamDescription streamDesc;
    streamDesc.mIsShared = true;
    streamDesc.mUsage = GHVBUsage::STATIC;
    streamDesc.mComps.push_back(GHVertexComponent::AP_POS);
    //comps.push_back(GHVertexComponent::AP_ENCODEDNORMAL);
    streamDesc.mComps.push_back(GHVertexComponent::AP_NORMAL);
    ret->addStreamDescription(streamDesc);

    return ret;
}
