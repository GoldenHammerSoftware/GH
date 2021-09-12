// Copyright Golden Hammer Software
#include "GHAMAnimFactoryGPU.h"
#include "GHGPUVertexAnimController.h"
#include "GHString/GHStringIdFactory.h"
#include "GHVBAnimData.h"
#include "GHTransformAnimController.h"
#include "GHTransformAnimData.h"
#include "GHSkinAnimControllerCPU.h"
#include "GHTransformNode.h"
#include "GHSkeletonAnimController.h"

GHAMAnimFactoryGPU::GHAMAnimFactoryGPU(const GHTimeVal& time, GHStringIdFactory& hashTable)
: mTime(time)
, mInterpProp(hashTable.generateHash("interppct"))
{
}

GHFrameAnimControllerVB* GHAMAnimFactoryGPU::createMeshAnim(void) const
{
    unsigned int lowStreamTarget = 1;
    unsigned int highStreamTarget = 2;
    unsigned int sourceStreamId = 0;
    
    GHFrameAnimControllerVB* ret =  new GHGPUVertexAnimController(lowStreamTarget, highStreamTarget,
                                                                  sourceStreamId, mInterpProp,
                                                                  mTime);
    GHVBAnimData* animData = new GHVBAnimData;
	ret->setAnimData(animData);
    return ret;
}

GHTransformAnimController* GHAMAnimFactoryGPU::createNodeAnim(void) const
{
    GHTransformAnimController* ret = new GHTransformAnimController(mTime);
    ret->setAnimData(new GHTransformAnimData);
    return ret;
}

GHSkinAnimController* GHAMAnimFactoryGPU::createSkinAnim(const std::vector<GHIdentifier>& nodeIds) const
{
    return new GHSkinAnimControllerCPU(nodeIds);
}

GHSkeletonAnimController* GHAMAnimFactoryGPU::createSkeletonAnim(GHTransformNode& skelTop) const
{
    std::vector<GHTransformNode*> flattenedTree;
    skelTop.collectTree(flattenedTree);
    std::vector<GHTransformAnimData*> animData;
    animData.resize(flattenedTree.size());
    for (size_t i = 0; i < animData.size(); ++i)
    {
        animData[i] = new GHTransformAnimData;
    }
    GHSkeletonAnimController* ret = new GHSkeletonAnimController(mTime);
    ret->setTarget(flattenedTree, animData);
    return ret;
}

void GHAMAnimFactoryGPU::modifyStreamDesc(const GHAM::MeshHeader& meshHeader, GHVBDescription::StreamDescription& streamDesc) const
{
    streamDesc.mIsShared = true;
    streamDesc.mUsage = GHVBUsage::STATIC;
    // pos stream in skinned meshes needs to be unique if using cpu skinning
    if (meshHeader.mIsSkinned) {
        bool streamHasPos = false;
        for (size_t i = 0; i < streamDesc.mComps.size(); ++i) {
            if (streamDesc.mComps[i].mID == GHVertexComponentShaderID::SI_POS) {
                streamHasPos = true;
            }
        }
        if (streamHasPos) {
            streamDesc.mIsShared = false;
            streamDesc.mUsage = GHVBUsage::DYNAMIC;
        }
    }
}
