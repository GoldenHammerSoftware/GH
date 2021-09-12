// Copyright Golden Hammer Software
#include "GHModelParticleAnimCloner.h"
#include "GHModel.h"
#include "GHParticleAnimController.h"

GHModelParticleAnimCloner::GHModelParticleAnimCloner(GHParticleAnimController& con)
: mCon(con)
{
}

void GHModelParticleAnimCloner::clone(const GHModel& srcModel, const std::vector<GHTransformNode*>& srcNodes,
	GHModel& cloneModel, std::vector<GHTransformNode*>& cloneNodes)
{
	GHParticleAnimController* retCon = (GHParticleAnimController*)mCon.clone();
	if (mCon.getTarget())
	{
		for (size_t i = 0; i < srcNodes.size(); ++i)
		{
			if (srcNodes[i] == mCon.getTarget())
			{
				retCon->setTarget(cloneNodes[i]);
				break;
			}
		}
	}
	cloneModel.addAnimController(retCon, new GHModelParticleAnimCloner(*retCon));
}
