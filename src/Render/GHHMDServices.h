#pragma once

class GHAppShard;
class GHController;
class GHTransform;
class GHTimeVal;

class GHHMDServices
{
public:
    virtual ~GHHMDServices(void) {}
    
	virtual void initAppShard(GHAppShard& appShard, const GHTransform& hmdOrigin, const GHTimeVal& realTimeVal) = 0;
	virtual GHController* createHMDTransformApplier(GHTransform& outTrans) = 0;
};

