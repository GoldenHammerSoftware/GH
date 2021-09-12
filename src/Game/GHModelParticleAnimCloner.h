// Copyright Golden Hammer Software
#pragma once

#include "GHModelAnimControllerCloner.h"

class GHParticleAnimController;

class GHModelParticleAnimCloner : public GHModelAnimControllerCloner
{
public:
	GHModelParticleAnimCloner(GHParticleAnimController& con);

	virtual void clone(const GHModel& srcModel, const std::vector<GHTransformNode*>& srcNodes,
		GHModel& cloneModel, std::vector<GHTransformNode*>& cloneNodes);

private:
	GHParticleAnimController& mCon;
};
