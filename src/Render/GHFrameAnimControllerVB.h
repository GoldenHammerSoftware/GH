// Copyright Golden Hammer Software
#pragma once

#include "GHFrameAnimController.h"
#include "GHNewFrameChecker.h"

class GHVBAnimData;
class GHGeometry;

class GHFrameAnimControllerVB : public GHFrameAnimController
{
public:
    GHFrameAnimControllerVB(void);
    ~GHFrameAnimControllerVB(void);
    
    virtual void setTarget(GHGeometry* target);
    const GHGeometry* getTarget(void) const { return mTarget; }
    
    void setAnimData(GHVBAnimData* data);
    GHVBAnimData* getAnimData(void) { return mAnimData; }
    
protected:
    GHVBAnimData* mAnimData;
    GHGeometry* mTarget;
    GHNewFrameChecker mFrameChecker;
};
