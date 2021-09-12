// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHTimeVal;

// starts a profile on activate and stops it after a period of time or a deactivate.
class GHProfileController : public GHController
{
public:
	GHProfileController(const GHTimeVal& time, float profileLen);

	virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

private:
	const GHTimeVal& mTime;
	// if <= 0 then deactivate on exit.
	float mProfileLen{ -1.0f };
	// do we have a profile running?
	bool mProfiling{ false };
	float mProfileStartTime{ 0.0f };
};

