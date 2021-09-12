// Copyright Golden Hammer Software
#pragma once

#include "GHFrameAnimControllerVB.h"
#include "GHVertexStream.h"
#include "GHString/GHIdentifier.h"

class GHPropertyContainer;
class GHTimeVal;

// A class that replaces vertex streams on a target buffer and sets an interpolation val.
class GHGPUVertexAnimController : public GHFrameAnimControllerVB
{
public:
    GHGPUVertexAnimController(unsigned int lowStreamTarget, unsigned int highStreamTarget,
                              unsigned int sourceStreamId, const GHIdentifier& interpPctProp,
                              const GHTimeVal& time);
    ~GHGPUVertexAnimController(void);

    virtual void update(void);
    
    virtual GHAnimController* clone(void) const;
    virtual void setTarget(GHGeometry* target);
    
    virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}
    
private:
    const GHTimeVal& mTime;
    unsigned int mLowTarget;
    unsigned int mHighTarget;
    unsigned int mSourceStreamId;
    GHVertexStreamPtr* mReplacementStream;
    GHIdentifier mInterpPctProp;
    GHPropertyContainer* mTargetProperties;
};
