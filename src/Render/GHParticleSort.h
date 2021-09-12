// Copyright Golden Hammer Software
#pragma once

// sort functions for ghparticles.

#include "GHParticle.h"

struct GHRenderParticleLessNoDist
{
	bool operator()(const GHParticle* node1, const GHParticle* node2) const
	{
		// sanity check.
		if (!node1 || !node2)
		{
			if (!node1 && !node2) return false;
			return node2 < node1;
		}

		if (node1->isDead() && !node2->isDead()) return false;
		if (node2->isDead() && !node1->isDead()) return true;
		return node1 < node2;
	}
};

struct GHRenderParticleLessFrontToBack
{
	bool operator()(const GHParticle* node1, const GHParticle* node2) const
	{
		// sanity check.
		if (!node1 || !node2)
		{
			if (!node1 && !node2) return false;
			return node2 < node1;
		}

		if (node1->isDead() && !node2->isDead()) return false;
		if (node2->isDead() && !node1->isDead()) return true;

		if (node1->getDistFromCam() < node2->getDistFromCam()) return true;
		if (node2->getDistFromCam() < node1->getDistFromCam()) return false;

		return node1 < node2;
	}
};

struct GHRenderParticleLessBackToFront
{
	bool operator()(const GHParticle* node1, const GHParticle* node2) const
	{
		// sanity check.
		if (!node1 || !node2)
		{
			if (!node1 && !node2) return false;
			return node2 < node1;
		}

		if (node1->isDead() && !node2->isDead()) return false;
		if (node2->isDead() && !node1->isDead()) return true;

		if (node1->getDistFromCam() > node2->getDistFromCam()) return true;
		if (node2->getDistFromCam() > node1->getDistFromCam()) return false;

		return node1 < node2;
	}
};
