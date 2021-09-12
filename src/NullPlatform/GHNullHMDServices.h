#pragma once
#include "GHHMDServices.h"

class GHNullHMDServices : public GHHMDServices
{
public:
	virtual void initAppShard(GHAppShard& appShard, const GHTransform& hmdOrigin, const GHTimeVal& realTimeVal);
	virtual GHController* createHMDTransformApplier(GHTransform& outTrans);
};

