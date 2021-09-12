// Copyright Golden Hammer Software
#include "GHSkeletonAnimController.h"
#include <assert.h>
#include "GHTransformNode.h"
#include "GHTransformAnimData.h"
#include "GHTransformAnimHelper.h"
#include "GHPlatform/GHTimeVal.h"

GHSkeletonAnimController::GHSkeletonAnimController(const GHTimeVal& time)
: mTime(time)
{
}

GHSkeletonAnimController::~GHSkeletonAnimController(void)
{
	for (size_t i = 0; i < mAnimData.size(); ++i) {
        mAnimData[i]->release();
    }
}

void GHSkeletonAnimController::setTarget(const std::vector<GHTransformNode*>& tree,
                                         const std::vector<GHTransformAnimData*>& animData)
{
	for (size_t i = 0; i < mAnimData.size(); ++i) {
        mAnimData[i]->release();
    }
    
    assert(tree.size() == animData.size());
    
    mAnimData = animData;
    mSkeleton = tree;
    
	for (size_t i = 0; i < mAnimData.size(); ++i) {
        assert(mAnimData[i]);
        mAnimData[i]->acquire();
    }
}

GHAnimController* GHSkeletonAnimController::clone(void) const
{
    GHSkeletonAnimController* ret = new GHSkeletonAnimController(mTime);
    ret->setAnimSet(mAnimSet);
    return ret;
}

void GHSkeletonAnimController::update(void)
{
    if (!mFrameChecker.checkIsNewFrame(mTime)) return;
    if (!mAnimBlends.size()) return;
    if (!mAnimBlends[0].mSequence) return;

    GHTransformNode* topNode = mSkeleton[0]->getParent();
    if (topNode) {
        topNode->removeChild(mSkeleton[0]);
    }
    GHTransform oldTrans = mSkeleton[0]->getLocalTransform();
    mSkeleton[0]->getLocalTransform().becomeIdentity();
    
	for (size_t i = 0; i < mSkeleton.size(); ++i) {
        updateNode(mSkeleton[i], mAnimData[i]);
    }

    mSkeleton[0]->getLocalTransform() = oldTrans;
    if (topNode) {
        topNode->addChild(mSkeleton[0]);
    }
}

GHTransformAnimData* GHSkeletonAnimController::getAnimForNode(const GHIdentifier& nodeId)
{
	for (size_t i = 0; i < mSkeleton.size(); ++i) {
        if (mSkeleton[i]->getId() == nodeId)
        {
            return mAnimData[i];
        }
    }
    return 0;
}

void GHSkeletonAnimController::updateNode(GHTransformNode* node, GHTransformAnimData* animData)
{
    if (!node || !animData) {
        return;
    }
    
    GHTransformAnimHelper animHelper;
    if (animHelper.blendTransform(mAnimBlends, mTime, *animData, node->getLocalTransform().getMatrix()))
    {
        // convert back from model space to local space.
        if (node->getParent()) {
            GHTransform parentTrans = node->getParent()->getTransform();
            parentTrans.invert();
            node->getLocalTransform().mult(parentTrans, node->getLocalTransform());
        }
        
        node->getLocalTransform().incrementVersion();
    }
}

void GHSkeletonAnimController::convertToModelSpace(void)
{
    if (!mAnimData.size() || !mSkeleton.size()) return;
    GHTransformNode* dummyNode = mSkeleton[0]->clone(0);
    std::vector<GHTransformNode*> flatDummy;
    dummyNode->collectTree(flatDummy);
    
    size_t maxFrames = 0;
    for (size_t i = 0; i < mAnimData.size(); ++i) {
        if (mAnimData[i]->getFrames().size() > maxFrames) {
            maxFrames = mAnimData[i]->getFrames().size();
        }
    }
    
	for (size_t frameId = 0; frameId < maxFrames; ++frameId)
    {
		for (size_t nodeId = 0; nodeId < mAnimData.size(); ++nodeId)
        {
            if (!mAnimData[nodeId]) continue;
            GHPoint16* frame = mAnimData[nodeId]->getFrame(frameId);
            if (!frame) continue;
            
            flatDummy[nodeId]->getLocalTransform().getMatrix() = *frame;
            flatDummy[nodeId]->getLocalTransform().incrementVersion();
            *mAnimData[nodeId]->getFrame(frameId) = flatDummy[nodeId]->getTransform().getMatrix();
        }
    }
    
    delete dummyNode;
}

