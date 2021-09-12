// Copyright Golden Hammer Software
#pragma once

#include "GHFrameAnimController.h"
#include "GHNewFrameChecker.h"
#include "GHMath/GHPoint.h"

class GHTransformNode;
class GHTransformAnimData;
class GHTimeVal;
 
// a controller that modifies a transformnode 
class GHTransformAnimController : public GHFrameAnimController
{
public:
    GHTransformAnimController(const GHTimeVal& time);
    ~GHTransformAnimController(void);
    
    virtual void update(void);
    
    void setTarget(GHTransformNode* target);
    const GHTransformNode* getTarget(void) const { return mTarget; }
    
    void setAnimData(GHTransformAnimData* data);
    GHTransformAnimData* getAnimData(void) { return mAnimData; }
    
    virtual GHAnimController* clone(void) const;

    virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}
    
private:
    void blendAnimFrames(const GHPoint16& lowMatrix, const GHPoint16& highMatrix, float pctLow, GHPoint16& matrixRes);

private:
    const GHTimeVal& mTime;
    GHTransformAnimData* mAnimData;
    GHTransformNode* mTarget;
    GHNewFrameChecker mFrameChecker;
};
