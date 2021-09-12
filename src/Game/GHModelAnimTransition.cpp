// Copyright Golden Hammer Software
#include "GHModelAnimTransition.h"
#include "GHMath/GHRandom.h"
#include "GHModel.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHMath/GHRandom.h"

GHModelAnimTransition::GHModelAnimTransition(GHModel& model, const GHIdentifier& anim,
                                             const GHTimeVal& time, float timeRandom)
: mModel(model)
, mTime(time)
, mTimeRandom(timeRandom)
{
    mAnims.push_back(anim);
}

void GHModelAnimTransition::activate(void)
{
    int randIndex = GHRandom::getRandInt((int)mAnims.size());
    float startTime = mTime.getTime() + GHRandom::getRandFloat(mTimeRandom);
	mModel.setAnim(mAnims[randIndex], startTime);
}
