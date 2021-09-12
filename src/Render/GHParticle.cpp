// Copyright Golden Hammer Software
#include "GHParticle.h"

GHParticle::GHParticle(void)
: mInitialAlpha(1)
{
	reset();
}

GHParticle::~GHParticle(void)
{
}

void GHParticle::reset(void)
{
	mRadius = 1;
	mStartTime = 0;
	mEndTime = 0;
	mDead = true;
	mAlpha = mInitialAlpha;
}
