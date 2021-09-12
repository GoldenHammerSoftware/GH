#pragma once

#ifdef GH_OVR_GO

#include "GHUtils/GHController.h"
#include "VrApi.h"
#include "VrApi_Helpers.h"
#include "VrApi_SystemUtils.h"
#include "GHOvrGoInputState.h"
#include "GHOvrGoInputTranslator.h"

class GHOvrGoFrameState;
class GHInputState;

// once per frame updates for ovr go
class GHOvrGoSystemController : public GHController
{
public:
	GHOvrGoSystemController(GHOvrGoFrameState& frameState, GHInputState& inputState, const GHTransform& hmdOrigin);
	~GHOvrGoSystemController(void);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	const GHOvrGoInputState& getOvrInputState(void) const { return mOvrInputState; }

private:
	void updateOvrFrameState(void);
	void pollInput(void);

private:
	GHOvrGoFrameState& mFrameState;
	GHInputState& mGHInputState;
	GHOvrGoInputState mOvrInputState;
	GHOvrGoInputTranslator mInputTranslator;
};

#endif