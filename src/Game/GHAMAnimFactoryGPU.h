// Copyright Golden Hammer Software
#pragma once

#include "GHAMAnimFactory.h"
#include "GHString/GHIdentifier.h"

class GHTimeVal;
class GHStringIdFactory;

// creates a GHGPUVertexAnimController
class GHAMAnimFactoryGPU : public GHAMAnimFactory
{
public:
    GHAMAnimFactoryGPU(const GHTimeVal& time, GHStringIdFactory& hashTable);

    virtual GHFrameAnimControllerVB* createMeshAnim(void) const;
    virtual GHTransformAnimController* createNodeAnim(void) const;
    virtual GHSkinAnimController* createSkinAnim(const std::vector<GHIdentifier>& nodeIds) const;
    virtual GHSkeletonAnimController* createSkeletonAnim(GHTransformNode& skelTop) const;
    virtual void modifyStreamDesc(const GHAM::MeshHeader& meshHeader, GHVBDescription::StreamDescription& streamDesc) const;
    
private:
    const GHTimeVal& mTime;
    GHIdentifier mInterpProp;
};
