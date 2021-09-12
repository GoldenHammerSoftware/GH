#pragma once

class btManifoldPoint;

class GHBulletContactAddedOverride
{
public:
	GHBulletContactAddedOverride(void);
	virtual ~GHBulletContactAddedOverride(void) { }

private:
	void registerContactAddedCallback(void);
};

class GHBulletContactAddedFrictionOverride : public GHBulletContactAddedOverride
{
public:

	virtual float getFriction(btManifoldPoint& cp, bool weAreFirstObj) = 0;

	virtual GHBulletContactAddedFrictionOverride* clone(void) = 0;
};


