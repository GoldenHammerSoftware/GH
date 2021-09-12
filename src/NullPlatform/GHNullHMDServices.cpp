#include "GHNullHMDServices.h"
#include "GHNullController.h"

void GHNullHMDServices::initAppShard(GHAppShard& appShard, const GHTransform& hmdOrigin, const GHTimeVal& realTimeVal)
{

}

GHController* GHNullHMDServices::createHMDTransformApplier(GHTransform& outTrans)
{
	return new GHNullController;
}
