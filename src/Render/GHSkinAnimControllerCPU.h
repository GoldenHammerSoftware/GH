// Copyright Golden Hammer Software
#pragma once

#include "GHSkinAnimController.h"
#include "GHString/GHIdentifier.h"
#include <vector>
#include "GHMath/GHTransform.h"
#include "GHVertexStream.h"

class GHVertexBuffer;
class GHTransformNode;

// an anim controller for skinning a mesh on the CPU
class GHSkinAnimControllerCPU : public GHSkinAnimController
{
public:
    // for creating the first instance.
    GHSkinAnimControllerCPU(const std::vector<GHIdentifier>& boneIds);
    // for creating clone instances
    GHSkinAnimControllerCPU(const std::vector<GHIdentifier>& boneIds,
                            GHVertexStreamPtr& srcPosStream,
                            const std::vector<GHTransform>& rootTransforms);
    ~GHSkinAnimControllerCPU(void);

    virtual void setTarget(GHVertexBuffer* targetVB, GHTransformNode* targetSkeleton);
    virtual const GHVertexBuffer* getTargetVB(void) const { return mTargetVB; }
    
	virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}
    
    virtual GHAnimController* clone(void) const;
    
private:
    GHVertexBuffer* mTargetVB;
    GHTransformNode* mTargetSkeleton;
    // copy of the pos stream so we have original position vals.
    GHVertexStreamPtr* mSrcPosStream;
    
    // ordered list of bone names that we expect to find in the mesh
    std::vector<GHIdentifier> mBoneIds;
    // the pose transforms for each bone, in model space.
    // indexed by order in mBoneIds.
    std::vector<GHTransform> mRootTransforms;
    // the inverse of the pose transforms for the bones.
    // used to bring the verts from model space into bone space.
    std::vector<GHTransform> mBoneInverseTransforms;
    // handle to the current state of each bone.
    // indexed by order in mBoneIds.
    std::vector<GHTransformNode*> mBones;
    
    // stream index of the stream with pos/normal
    unsigned int mPosStreamId;
    // stream index of the stream with boneidx/boneweight
    unsigned int mBoneStreamId;
};
