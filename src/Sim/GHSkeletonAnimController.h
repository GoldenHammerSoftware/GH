// Copyright Golden Hammer Software
#pragma once

#include "GHFrameAnimController.h"
#include <vector>
#include "GHNewFrameChecker.h"

class GHTransformNode;
class GHTransformAnimData;
class GHTimeVal;

// animates an entire skeleton in model space.
// convert anim local frames to model space, blend, then convert back to local space.
class GHSkeletonAnimController : public GHFrameAnimController
{
public:
    GHSkeletonAnimController(const GHTimeVal& time);
    ~GHSkeletonAnimController(void);
    
    void setTarget(const std::vector<GHTransformNode*>& tree,
                   const std::vector<GHTransformAnimData*>& animData);
    
    virtual GHAnimController* clone(void) const;
    
    virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}
    
    GHTransformAnimData* getAnimForNode(const GHIdentifier& nodeId);
    const std::vector<GHTransformAnimData*>& getAnimData(void) const { return mAnimData; }
    
    // sometimes our anim data is in local space and we want to convert it to model space.
    void convertToModelSpace(void);
    
private:
    void updateNode(GHTransformNode* node, GHTransformAnimData* animData);

private:
    std::vector<GHTransformNode*> mSkeleton;
    std::vector<GHTransformAnimData*> mAnimData;
    const GHTimeVal& mTime;
    GHNewFrameChecker mFrameChecker;
};
