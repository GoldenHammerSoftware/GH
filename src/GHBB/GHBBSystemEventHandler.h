#pragma once

#include <bps/event.h>

class GHMessageHandler;
class GHInputState;
class GHScreenInfo;
class GHBBGLESContext;

class GHBBSystemEventHandler
{
public:
	GHBBSystemEventHandler(GHInputState& inputState, GHMessageHandler& appMessageHandler,
			GHScreenInfo& screenInfo, bool allowsRotate,
			GHBBGLESContext& context);
	bool isPaused(void) const { return mIsPaused; }

	void handleScreenEvent(bps_event_t *event);
	void handleSensorEvent(bps_event_t* event);
	void handleNavigatorEvent(bps_event_t* event);

private:
	void calculateAccelForceMultiplier(float dir);
private:
	GHInputState& mInputState;
	GHMessageHandler& mAppMessageHandler;
	GHScreenInfo& mScreenInfo;
	bool mAllowsRotate;
	GHBBGLESContext& mContext;

	float mAccelForceMultiplier;
	bool mIsPaused;

	// playbook and bb10 alpha have different orientations
	bool mAccelSwapped;
};
