// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

class GHParticleSystem;

// transition that turns on/off a particle system.
class GHParticleTransition : public GHTransition
{
public:
	GHParticleTransition(GHParticleSystem& particle);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	GHParticleSystem& mParticle;
};
