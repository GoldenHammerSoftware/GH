#include "GHSkinAnimControllerCPU.h"
#include "GHVertexBuffer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHTransformNode.h"
#include "GHUtils/GHProfiler.h"
#include <string.h>

GHSkinAnimControllerCPU::GHSkinAnimControllerCPU(const std::vector<GHIdentifier>& boneIds)
: mTargetVB(0)
, mBoneIds(boneIds)
, mPosStreamId(0)
, mBoneStreamId(0)
, mSrcPosStream(0)
, mTargetSkeleton(0)
{
}

GHSkinAnimControllerCPU::GHSkinAnimControllerCPU(const std::vector<GHIdentifier>& boneIds,
                                                 GHVertexStreamPtr& srcPosStream,
                                                 const std::vector<GHTransform>& rootTransforms)
: mTargetVB(0)
, mBoneIds(boneIds)
, mPosStreamId(0)
, mBoneStreamId(0)
, mSrcPosStream(&srcPosStream)
, mTargetSkeleton(0)
, mRootTransforms(rootTransforms)
{
    mSrcPosStream->acquire();

    mBoneInverseTransforms.resize(mBoneIds.size());
    for (size_t boneIdx = 0; boneIdx < mBoneIds.size(); ++boneIdx)
    {
        mBoneInverseTransforms[boneIdx] = mRootTransforms[boneIdx];
        mBoneInverseTransforms[boneIdx].invert();
    }
}

GHSkinAnimControllerCPU::~GHSkinAnimControllerCPU(void)
{
    if (mSrcPosStream) mSrcPosStream->release();
}

void GHSkinAnimControllerCPU::setTarget(GHVertexBuffer* targetVB,
                                        GHTransformNode* targetSkeleton)
{
    mTargetVB = targetVB;
    mTargetSkeleton = targetSkeleton;

    // grab direct pointers to the bones we use to animate.
    mBones.resize(mBoneIds.size());
    for (size_t boneIdx = 0; boneIdx < mBoneIds.size(); ++boneIdx)
    {
        GHTransformNode* targBone = targetSkeleton->findChild(mBoneIds[boneIdx]);
        assert(targBone);
        if (!targBone) {
            GHDebugMessage::outputString("Failed to find target bone for skinning");
        }
        mBones[boneIdx] = targBone;
    }

    // figure out which stream is the position and which stream is the bone weights.
    const std::vector<GHVertexStreamPtr*>& targStreams = mTargetVB->getStreams();
    int posStreamId = -1;
    int weightStreamId = -1;
    for (size_t streamId = 0; streamId < targStreams.size(); ++streamId)
    {
        const GHVertexStreamFormat& streamFormat = targStreams[streamId]->get()->getFormat();
        if (streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS))
        {
            posStreamId = (int)streamId;
        }
        if (streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_BONEIDX) &&
            streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_BONEWEIGHT))
        {
            weightStreamId = (int)streamId;
        }
    }
    assert(posStreamId != -1 && weightStreamId != -1);
    mPosStreamId = posStreamId;
    mBoneStreamId = weightStreamId;
    
    // make a copy of the original vert positions.
    if (!mSrcPosStream)
    {
        mSrcPosStream = new GHVertexStreamPtr(targStreams[posStreamId]->get()->clone());
        mSrcPosStream->acquire();

        // grab the original transform values off the bones so we can compare later.
        // also convert those into model space.
        GHTransform skelTransform = mTargetSkeleton->getTransform();
        skelTransform.invert();
        mRootTransforms.resize(mBoneIds.size());
        mBoneInverseTransforms.resize(mBoneIds.size());
        for (size_t boneIdx = 0; boneIdx < mBones.size(); ++boneIdx)
        {
            mRootTransforms[boneIdx] = mBones[boneIdx]->getTransform();
            mRootTransforms[boneIdx].mult(skelTransform, mRootTransforms[boneIdx]);
            mBoneInverseTransforms[boneIdx] = mRootTransforms[boneIdx];
            mBoneInverseTransforms[boneIdx].invert();
        }
    }
}

void GHSkinAnimControllerCPU::update(void)
{
    GHPROFILESCOPE("Skinning", GHString::CHT_REFERENCE)
    
    const std::vector<GHVertexStreamPtr*>& targStreams = mTargetVB->getStreams();
    GHVertexStream* posStream = targStreams[mPosStreamId]->get();
    const GHVertexStreamFormat& posFormat = posStream->getFormat();
    GHVertexStream* boneStream = targStreams[mBoneStreamId]->get();
    const GHVertexStreamFormat& boneFormat = boneStream->getFormat();
    
    const GHVertexStreamFormat::ComponentEntry* posComp = posFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS);
    const GHVertexStreamFormat::ComponentEntry* normComp = posFormat.getComponentEntry(GHVertexComponentShaderID::SI_NORMAL);
    const GHVertexStreamFormat::ComponentEntry* boneIdxComp = boneFormat.getComponentEntry(GHVertexComponentShaderID::SI_BONEIDX);
    const GHVertexStreamFormat::ComponentEntry* boneWeightComp = boneFormat.getComponentEntry(GHVertexComponentShaderID::SI_BONEWEIGHT);

    float* posBuf = posStream->lockWriteBuffer();
    const float* boneBuf = boneStream->lockReadBuffer();
    const float* srcBuf = mSrcPosStream->get()->lockReadBuffer();
    
    // create the bone diff transforms.
    std::vector<GHTransform> boneDiffs;
    boneDiffs.resize(mBoneInverseTransforms.size());
    std::vector<GHTransform> skinTrans;
    skinTrans.resize(mBoneInverseTransforms.size());
    GHTransform skelTransform = mTargetSkeleton->getTransform();
    skelTransform.invert();
    for (size_t boneIdx = 0; boneIdx < boneDiffs.size(); ++boneIdx)
    {
        boneDiffs[boneIdx] = mBones[boneIdx]->getTransform();
        // convert the current frame bone world position into model space.
        boneDiffs[boneIdx].mult(skelTransform, boneDiffs[boneIdx]);
        // convert the model space current frame bone into original-pose bone space.
        boneDiffs[boneIdx].mult(mBoneInverseTransforms[boneIdx], boneDiffs[boneIdx]);

        // convert pos/norm into bone space
        skinTrans[boneIdx] = mBoneInverseTransforms[boneIdx];
        // multiply it by the bone space diff of current pose to original pose
        skinTrans[boneIdx].mult(boneDiffs[boneIdx], skinTrans[boneIdx]);
        // convert it back into model space.
        skinTrans[boneIdx].mult(mRootTransforms[boneIdx], skinTrans[boneIdx]);
    }
    
    GHPoint3 tempPos(0,0,0);
    GHPoint3 totPos(0,0,0);
    GHPoint3 srcPos(0,0,0);
    GHPoint3 totNorm(0,0,0);
    GHPoint3 srcNorm(0,0,0);
    const size_t pos3Size = sizeof(float)*3;
    //GHPROFILEBEGIN("SkinLoop")
    for (unsigned int vertId = 0; vertId < posStream->getNumVerts(); ++vertId)
    {
        float* posStart = posBuf + posComp->mOffset;
        const float* srcPosStart = srcBuf + posComp->mOffset;
        srcPos.setCoords(srcPosStart);

        const unsigned char* boneIdxStart = (unsigned char*)(boneBuf + boneIdxComp->mOffset);
        const float* boneWeightStart = boneBuf + boneWeightComp->mOffset;
        
        float* normStart = 0;
        const float* srcNormStart = 0;
        if (normComp) {
            normStart = posBuf + normComp->mOffset;
            srcNormStart = srcBuf + normComp->mOffset;
            srcNorm.setCoords(srcNormStart);
        }
        
        // do magic.
        float totWeight = 0;
        for (size_t boneId = 0; boneId < boneIdxComp->mCount; ++boneId)
        {
            skinTrans[boneIdxStart[boneId]].mult(srcPos, tempPos);
            tempPos *= boneWeightStart[boneId];
            totWeight += boneWeightStart[boneId];
            totPos += tempPos;
        
            if (normComp)
            {
                skinTrans[boneIdxStart[boneId]].multDir(srcNorm, tempPos);
                tempPos *= boneWeightStart[boneId];
                totNorm += tempPos;
            }
        }
        
        if (totWeight != 0)
        {
            totPos /= totWeight;
            memcpy(posStart, totPos.getCoords(), pos3Size);
            
            if (normComp)
            {
                totNorm /= totWeight;
                memcpy(normStart, totNorm.getCoords(), pos3Size);
            }
        }
        
		// test hack.
		if (totPos[0] > 2 || totPos[1] > 2 || totPos[2] > 2)
		{
			GHDebugMessage::outputString("exploding skin");
		}

        posBuf += posFormat.getVertexSize();
        boneBuf += boneFormat.getVertexSize();
        srcBuf += posFormat.getVertexSize();
        totPos.setCoords(0,0,0);
        totNorm.setCoords(0,0,0);
    }
    //GHPROFILEEND("SkinLoop")
    
    boneStream->unlockReadBuffer();
    posStream->unlockWriteBuffer();
    mSrcPosStream->get()->unlockReadBuffer();
}

GHAnimController* GHSkinAnimControllerCPU::clone(void) const
{
    GHSkinAnimControllerCPU* ret = new GHSkinAnimControllerCPU(mBoneIds, *mSrcPosStream, mRootTransforms);
    return ret;
}
