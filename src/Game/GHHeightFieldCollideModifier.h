// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

class GHHeightField;
class GHPhysicsSim;

// a transition that collides a heightfield with a physics world,
// and replaces heights in the field with the collision values.
class GHHeightFieldCollideModifier : public GHTransition
{
public:
	GHHeightFieldCollideModifier(GHHeightField& hf, const GHPhysicsSim& physics, 
		float worldHeight, int collisionLayer, 
		unsigned int edgeSmooth, float edgeSmoothPct,
		unsigned int colSmooth, float colSmoothPct, 
		GHHeightField* noiseHF);
	~GHHeightFieldCollideModifier(void);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	void addCollisionNoise(bool* collided, unsigned int nodeWidth, unsigned int nodeHeight);
	void smoothCollisionEdges(bool* collided, unsigned int nodeWidth, unsigned int nodeHeight);

private:
	GHHeightField& mHF;
	const GHPhysicsSim& mPhysics;
	int mCollisionLayer;
	// the height we use for casting collision rays.
	// should be higher than any point in the world.
	float mWorldHeight;
	// number of non colliding edges to smooth.  0 if no smoothing.
	unsigned int mEdgeSmooth;
	// number of colliding edges to smooth.  0 if no smoothing.
	unsigned int mColSmooth;
	// how close do we move collide nodes to the ave
	float mColSmoothPct;
	// how close do we move the non-collide nodes to the ave
	float mEdgeSmoothPct;
	// an optional hf for adding noise to nodes that collide.
	// we own noise hf.
	GHHeightField* mNoiseHF;
};
