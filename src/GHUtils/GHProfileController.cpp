// Copyright Golden Hammer Software
#include "GHProfileController.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHUtils/GHProfiler.h"

GHProfileController::GHProfileController(const GHTimeVal& time, float profileLen)
	: mTime(time)
	, mProfileLen(profileLen)
{
}

void GHProfileController::update(void)
{
	if (mProfiling && mProfileLen > 0.0f)
	{
		float len = mTime.getTime() - mProfileStartTime;
		if (len > mProfileLen)
		{
			GHPROFILESTOP
			GHPROFILEOUTPUT
			GHPROFILECLEAR
			mProfiling = false;
		}
	}
}

void GHProfileController::onActivate(void)
{
	mProfiling = true;
	mProfileStartTime = mTime.getTime();
	GHPROFILESTART
}

void GHProfileController::onDeactivate(void)
{
	if (mProfiling)
	{
		GHPROFILESTOP
		GHPROFILEOUTPUT
		GHPROFILECLEAR
	}
}
