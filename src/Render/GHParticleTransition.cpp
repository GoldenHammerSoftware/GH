// Copyright Golden Hammer Software
#include "GHParticleTransition.h"
#include "GHParticleSystem.h"

GHParticleTransition::GHParticleTransition(GHParticleSystem& particle)
: mParticle(particle)
{
	mParticle.getSpawner().setSpawningAllowed(false);
}

void GHParticleTransition::activate(void)
{
	if (mParticle.isDead()) {
		mParticle.revive();
	}
	mParticle.getSpawner().setSpawningAllowed(true);
}

void GHParticleTransition::deactivate(void)
{
	mParticle.getSpawner().setSpawningAllowed(false);
}
